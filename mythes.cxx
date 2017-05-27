#include "COPYING"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits>
#include <vector>

#include "mythes.hxx"

MyThes::MyThes(const char* idxpath, const char * datpath)
{
    nw = 0;
    encoding = NULL;
    list = NULL;
    offst = NULL;
    pdfile = NULL;

    if (thInitialize(idxpath, datpath) != 1) {
        fprintf(stderr,"Error - can't open %s or %s\n",idxpath, datpath);
        fflush(stderr);
        thCleanup();
        // did not initialize properly - throw exception?
    }
}


MyThes::~MyThes()
{
    thCleanup();
}


int MyThes::thInitialize(const char* idxpath, const char* datpath)
{

    // open the index file
    FILE * pifile = fopen(idxpath,"r");
    if (!pifile) {
        return 0;
    } 

    // parse in encoding and index size */    
    std::vector<char> buffer(MAX_WD_LEN);
    char * wrd = &buffer[0];
    readLine(pifile,wrd,MAX_WD_LEN);
    encoding = mystrdup(wrd);
    readLine(pifile,wrd,MAX_WD_LEN);
    int idxsz = atoi(wrd); 
   
    if (idxsz <= 0 || idxsz > std::numeric_limits<int>::max() / sizeof(char*)) {
       fprintf(stderr,"Error - bad index %d\n", idxsz);
       fclose(pifile);
       return 0;
    }

    // now allocate list, offst for the given size
    list = (char**)   calloc(idxsz,sizeof(char*));
    offst = (unsigned int*) calloc(idxsz,sizeof(unsigned int));

    if ( (!(list)) || (!(offst)) ) {
       fprintf(stderr,"Error - bad memory allocation\n");
       fclose(pifile);
       return 0;
    }

    // now parse the remaining lines of the index
    int len = readLine(pifile,wrd,MAX_WD_LEN);
    while (len > 0)
    { 
        int np = mystr_indexOfChar(wrd,'|');
        if (nw < idxsz) {
            if (np >= 0) {          
                *(wrd+np) = '\0';
                list[nw] = (char *)calloc(1,(np+1));
                if (!list[nw]) {
                    fprintf(stderr,"Error - bad memory allocation\n");
                    fflush(stderr);
                    fclose(pifile);
                    return 0;
                }
                memcpy((list[nw]),wrd,np);
                offst[nw] = atoi(wrd+np+1);
                nw++;
            }
        }
        len = readLine(pifile,wrd,MAX_WD_LEN);
    }

    fclose(pifile);

    /* next open the data file */
    pdfile = fopen(datpath,"r");
    if (!pdfile) {
        return 0;
    } 
        
    return 1;        
}


void MyThes::thCleanup()
{
    /* first close the data file */
    if (pdfile) {
        fclose(pdfile);
        pdfile=NULL;
    }

    if (list)
    {
        /* now free up all the allocated strings on the list */
        for (int i=0; i < nw; i++) 
        {
            if (list[i]) {
                free(list[i]);
                list[i] = 0;
            }
        }
        free((void*)list);
    }

    if (encoding) free((void*)encoding);
    if (offst) free((void*)offst);

    encoding = NULL;
    list = NULL;
    offst = NULL;
    nw = 0;
}



// lookup text in index and count of meanings and a list of meaning entries
// with each entry having a synonym count and pointer to an 
// array of char * (i.e the synonyms)
// 
// note: calling routine should call CleanUpAfterLookup with the original
// meaning point and count to properly deallocate memory

int MyThes::Lookup(const char * pText, int len, mentry** pme)
{ 

    *pme = NULL;

    // handle the case of missing file or file related errors
    if (! pdfile) return 0;

    long offset = 0;

    /* copy search word and make sure null terminated */
    std::vector<char> buffer(len+1);
    char * wrd = &buffer[0];
    memcpy(wrd,pText,len);
  
    /* find it in the list */
    int idx = nw > 0 ? binsearch(wrd,list,nw) : -1;
    if (idx < 0) return 0;

    // now seek to the offset
    offset = (long) offst[idx];
    int rc = fseek(pdfile,offset,SEEK_SET);
    if (rc) {
       return 0;
    }

    // grab the count of the number of meanings
    // and allocate a list of meaning entries
    char * buf = NULL;
    buf  = (char *) malloc( MAX_LN_LEN );
    if (!buf) return 0;
    readLine(pdfile, buf, (MAX_LN_LEN-1));
    int np = mystr_indexOfChar(buf,'|');
    if (np < 0) {
         free(buf);
         return 0;
    }          
    int nmeanings = atoi(buf+np+1);
    if (nmeanings < 0 || nmeanings > std::numeric_limits<int>::max() / sizeof(mentry))
        nmeanings = 0;
    *pme = (mentry*)(nmeanings ? malloc(nmeanings * sizeof(mentry)) : NULL);
    if (!(*pme)) {
        free(buf);
        return 0;
    }

    // now read in each meaning and parse it to get defn, count and synonym lists
    mentry* pm = *(pme);
    char dfn[MAX_WD_LEN];

    for (int j = 0; j < nmeanings; j++) {
        readLine(pdfile, buf, (MAX_LN_LEN-1));

        pm->count = 0;
        pm->psyns = NULL;
        pm->defn = NULL;

        // store away the part of speech for later use
        char * p = buf;
        char * pos = NULL;
        np = mystr_indexOfChar(p,'|');
        if (np >= 0) {
           *(buf+np) = '\0';
	   pos = mystrdup(p);
           p = p + np + 1;
	} else {
          pos = mystrdup("");
        }
        
        // count the number of fields in the remaining line
        int nf = 1;
        char * d = p;
        np = mystr_indexOfChar(d,'|');        
        while ( np >= 0 ) {
	  nf++;
          d = d + np + 1;
          np = mystr_indexOfChar(d,'|');          
	}
	pm->count = nf;
        pm->psyns = (char **) malloc(nf*sizeof(char*)); 
        
        // fill in the synonym list
        d = p;
        for (int jj = 0; jj < nf; jj++) 
        {
            np = mystr_indexOfChar(d,'|');
            if (np > 0) 
            {
                *(d+np) = '\0';
                pm->psyns[jj] = mystrdup(d);
                d = d + np + 1;
            } 
            else 
            {
              pm->psyns[jj] = mystrdup(d);
            }            
        }

        // add pos to first synonym to create the definition
        if (pm->psyns[0])
	{
            int k = strlen(pos);
            int m = strlen(pm->psyns[0]);
            if ((k+m) < (MAX_WD_LEN - 1)) {
                 strncpy(dfn,pos,k);
                 *(dfn+k) = ' ';
                 strncpy((dfn+k+1),(pm->psyns[0]),m+1);
                 pm->defn = mystrdup(dfn);
            } else {
                pm->defn = mystrdup(pm->psyns[0]);
            }
        }
        free(pos);
        pm++;

    }
    free(buf);
   
    return nmeanings;
} 



void MyThes::CleanUpAfterLookup(mentry ** pme, int nmeanings)
{ 

    if (nmeanings == 0) return;
    if ((*pme) == NULL) return;

    mentry * pm = *pme;
       
    for (int i = 0; i < nmeanings; i++) {
       int count = pm->count;
       for (int j = 0; j < count; j++) {
	  if (pm->psyns[j]) free(pm->psyns[j]);
          pm->psyns[j] = NULL;
       }
       if (pm->psyns) free(pm->psyns);
       pm->psyns = NULL;
       if (pm->defn) free(pm->defn);
       pm->defn = NULL;
       pm->count = 0;
       pm++;
    }
    pm = *pme;
    free(pm);
    *pme = NULL;
    return;
}


// read a line of text from a text file stripping
// off the line terminator and replacing it with
// a null string terminator.
// returns:  -1 on error or the number of characters in
//             in the returning string

// A maximum of nc characters will be returned

int MyThes::readLine(FILE * pf, char * buf, int nc)
{
    
  if (fgets(buf,nc,pf)) {
    mychomp(buf);
    return strlen(buf);
  }
  return -1;
}


 
//  performs a binary search on null terminated character
//  strings
//
//  returns: -1 on not found
//           index of wrd in the list[]

int MyThes::binsearch(char * sw, char* _list[], int nlst) 
{
    int lp, up, mp, j, indx;
    lp = 0;
    up = nlst-1;
    indx = -1;
    if (strcmp(sw,_list[lp]) < 0) return -1;
    if (strcmp(sw,_list[up]) > 0) return -1;
    while (indx < 0 ) {
        mp = (int)((lp+up) >> 1);
        j = strcmp(sw,_list[mp]);
        if ( j > 0) {
            lp = mp + 1;
        } else if (j < 0 ) {
            up = mp - 1;
        } else {
            indx = mp;
        }
        if (lp > up) return -1;      
    }
    return indx;
}

char * MyThes::get_th_encoding()
{
  return encoding;
}


// string duplication routine
char * MyThes::mystrdup(const char * s)
{
  char * d = NULL;
  if (s) {
    int sl = strlen(s)+1;
    d = (char *) malloc(sl);
    if (d) memcpy(d,s,sl);
  }
  return d;
}

// remove cross-platform text line end characters
void MyThes::mychomp(char * s)
{
  int k = strlen(s);
  if ((k > 0) && ((*(s+k-1)=='\r') || (*(s+k-1)=='\n'))) *(s+k-1) = '\0';
  if ((k > 1) && (*(s+k-2) == '\r')) *(s+k-2) = '\0';
}


// return index of char in string
int MyThes::mystr_indexOfChar(const char * d, int c)
{
  char * p = strchr((char *)d,c);
  if (p) return (int)(p-d);
  return -1;
}

