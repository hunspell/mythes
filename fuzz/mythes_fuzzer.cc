// libFuzzer harness for MyThes index/data parsing and lookup.
//
// The fuzz input is split as:
//
//     [uint16_t idx_len, little-endian][idx bytes][dat bytes]
//
// The two halves are written to temporary files and fed to MyThes. Words
// are then harvested from the synthesized .idx (lines past the encoding
// and count headers, up to the first '|') and looked up, so the per-meaning
// parse path in MyThes::Lookup actually gets exercised.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>

#include "mythes.hxx"

namespace {

struct ScopedTempFile {
    char path[32];
    int fd;

    explicit ScopedTempFile(const char *tag) : fd(-1) {
        std::snprintf(path, sizeof(path), "/tmp/mythes_fuzz_%s_XXXXXX", tag);
        fd = mkstemp(path);
    }

    ~ScopedTempFile() {
        if (fd >= 0) close(fd);
        unlink(path);
    }

    bool writeAll(const uint8_t *p, size_t n) const {
        while (n) {
            ssize_t w = write(fd, p, n);
            if (w <= 0) return false;
            p += static_cast<size_t>(w);
            n -= static_cast<size_t>(w);
        }
        return true;
    }
};

}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < 2) return 0;
    size_t idx_len = static_cast<size_t>(data[0]) | (static_cast<size_t>(data[1]) << 8);
    if (idx_len > size - 2) return 0;

    const uint8_t *idx = data + 2;
    const uint8_t *dat = idx + idx_len;
    size_t dat_len = size - 2 - idx_len;

    ScopedTempFile idx_file("idx");
    ScopedTempFile dat_file("dat");
    if (idx_file.fd < 0 || dat_file.fd < 0) return 0;
    if (!idx_file.writeAll(idx, idx_len)) return 0;
    if (!dat_file.writeAll(dat, dat_len)) return 0;

    MyThes mt(idx_file.path, dat_file.path);

    // Walk the .idx text and look up each word so the .dat parse path is
    // reached. Skip the first two lines (encoding, count) per the format.
    const char *p = reinterpret_cast<const char*>(idx);
    const char *end = p + idx_len;
    for (int skipped = 0; skipped < 2 && p < end; ++skipped) {
        while (p < end && *p != '\n') ++p;
        if (p < end) ++p;
    }
    // Cap probes per input: each Lookup parses one .dat record (potentially
    // thousands of allocations), so a high count amplifies any slow path
    // across the whole iteration and starves libFuzzer of exec/s.
    int probed = 0;
    while (p < end && probed < 4) {
        const char *line = p;
        while (p < end && *p != '\n') ++p;
        const char *line_end = p;
        if (p < end) ++p;
        const char *bar = static_cast<const char*>(
            std::memchr(line, '|', static_cast<size_t>(line_end - line)));
        if (!bar || bar == line) continue;
        std::string word(line, static_cast<size_t>(bar - line));
        mentry *pme = nullptr;
        int n = mt.Lookup(word.data(), static_cast<int>(word.size()), &pme);
        if (n > 0) mt.CleanUpAfterLookup(&pme, n);
        ++probed;
    }

    // A handful of literal probes catch the binsearch / not-found paths even
    // when the synthesized .idx has no usable lines.
    static const char * const literals[] = { "", "a", "test" };
    for (const char *w : literals) {
        mentry *pme = nullptr;
        int n = mt.Lookup(w, static_cast<int>(std::strlen(w)), &pme);
        if (n > 0) mt.CleanUpAfterLookup(&pme, n);
    }

    return 0;
}
