#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "mythes.hxx"

#include <hunspell.hxx>

extern char * mystrdup(const char * s);

int main(int argc, char** argv)
{
    FILE* wtclst;

  /* first parse the command line options
   * arg1 - index file, arg2 thesaurus data file, arg3 - file of words to check,
   * arg4, arg5 - opt. Hunspell affix and dic file for stemming and
   *              morphological generation 
   */

  if (argc < 3) {
    fprintf(stderr,"correct syntax is:\n"); 
    fprintf(stderr,"example index_file thesaurus_file file_of_words_to_check [affix_file dic_file]\n");
    exit(1);
  }

  /* open the words to check list */
  wtclst = fopen(argv[3], "r");
  if (!wtclst) {
    fprintf(stderr,"Error - could not open file of words to check\n");
    exit(1);
  }

    // Hunspell for stemming and morphological generation of affixes synonyms
    Hunspell * pH = NULL;    
    if (argc >= 5) pH = new Hunspell(argv[4], argv[5], (const char *) NULL);

    // open a new thesaurus object
    MyThes * pMT = new MyThes(argv[1], argv[2]);

    // get the encoding used for the thesaurus data
    char * encoding = pMT->get_th_encoding();
    printf("Thesaurus uses encoding %s\n\n", encoding);
    
    int k;
    char buf[101];
    char oldbuf[101];
    mentry * pmean;

    while(fgets(buf,100,wtclst)) {
      oldbuf[0] = '\0';
      k = strlen(buf);
      *(buf + k - 1) = '\0';
      int len = strlen(buf);
      int count = pMT->Lookup(buf,len,&pmean);
      // don't change value of pmean
      // or count since needed for CleanUpAfterLookup routine
      if (!count) {
        std::vector<std::string> stem;
        if (pH) stem = pH->stem(buf);
        if (!stem.empty()) {
            printf("stem: %s\n", stem[0].c_str());
            strncpy(oldbuf,buf, sizeof(oldbuf)-1);
            oldbuf[sizeof(oldbuf)-1] = 0;
            strncpy(buf, stem[0].c_str(), sizeof(buf)-1);
            buf[sizeof(buf)-1] = 0;
            len = strlen(buf);
            count = pMT->Lookup(buf, len, &pmean);
        } else oldbuf[0] = '\0';
      }

      mentry* pm = pmean;
      if (count) {
        printf("%s has %d meanings\n",buf,count);
	for (int  i=0; i < count; i++) {
          printf("   meaning %d: %s\n",i,pm->defn);
          for (int j=0; j < pm->count; j++) {
            std::vector<std::string> gen;
            if (pH && oldbuf[0]) gen = pH->generate(pm->psyns[j], oldbuf);
            if (!gen.empty()) {
                printf("       %s",gen[0].c_str());
                for (size_t k = 1, l = gen.size(); k < l; ++k) printf(", %s",gen[k].c_str());
                printf("\n");
	    } else printf("       %s\n",pm->psyns[j]);
	    
          }
          printf("\n");
          pm++;
	}
        printf("\n\n");
        // now clean up all allocated memory 
        pMT->CleanUpAfterLookup(&pmean,count);
      } else {
        printf("\"%s\" is not in thesaurus!\n",buf);
      }
    }

    fclose(wtclst);
    delete pMT;
    if (pH) delete pH;
    return 0;
}

