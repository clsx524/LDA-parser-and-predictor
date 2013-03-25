#include "strtokenizer.h"

using namespace std;

strtokenizer::strtokenizer(string str, string seperators, bool preEnable) {
    if (preEnable == true) {
        read_stop_list();
        preprocess(str);
    } else {
        parse(str, seperators);
    }
}

void strtokenizer::preprocess(string text) {
    text = verify(text);
    boost::regex re1("([a-z])([0-9])");
    boost::regex re2("([0-9])([a-z])");
    boost::regex re3("\\s+");
    text = boost::regex_replace(text, re1, "\\1 \\2");
    text = boost::regex_replace(text, re2, "\\1 \\2");
    text = boost::regex_replace(text, re3, " ");
    text = stem(z,text,(int)text.size());
    text = stopword_remover(text);
    tokens.push_back(text);
}

void strtokenizer::parse(string str, string seperators) {
    int n = str.length();
    int start, stop;
    
    start = str.find_first_not_of(seperators);
    while (start >= 0 && start < n) {
        stop = str.find_first_of(seperators, start);
        if (stop < 0 || stop > n) {
            stop = n;
        }
        
        tokens.push_back(str.substr(start, stop - start));
        start = str.find_first_not_of(seperators, stop + 1);
    }
    start_scan(); // set idx = 0
}

void strtokenizer::read_stop_list() {
    string src = "stop-words/stoplist-nsp.regex", str, tmp;
    ifstream fp;
    fp.open(src.c_str(), ifstream::in);
    for (char i = 'a'; i < 'z' + 1; ++i) {
        tmp.clear();
        tmp.copy(&i,1);
        stopList.push_back(tmp);
    }
    getline(fp,str);
    while (!fp.eof()) {
        getline(fp, str);
        str = str.substr(5,str.size()-3-5);
        str.erase(find(str.begin(), str.end(), ']'));
        transform(str.begin(),str.end(),str.begin(),::tolower);
        stopList.push_back(str);
    }
}

string strtokenizer::stopword_remover(string str) {
    for (vector<string>::size_type i = 0; i < stopList.size(); i++) {
        if (stopList[i] == str) {
            return "";
        } else {
            return str;
        }
    }
    return str;
}

string strtokenizer::verify(string str) {
    for (string::iterator it = str.begin(); it < str.end(); ++it) {
        if (toascii(*it) > 127) {
            str.erase(it);
        } else if (isdigit(*it) || isalpha(*it)) {
            continue;
        } else {
            *it = ' ';
        }
    }
    return str;
}

int strtokenizer::count_tokens() {
    return tokens.size();
}

void strtokenizer::start_scan() {
    idx = 0;
}

string strtokenizer::next_token() {
    if (idx >= 0 && idx < (int)tokens.size()) {
        return tokens[idx++];
    } else {
        return "";
    }
}

string strtokenizer::token(int i) {
    if (i >= 0 && i < (int)tokens.size()) {
        return tokens[i];
    } else {
        return "";
    }
}

/* cons(z, i) is true <=> b[i] is a consonant. ('b' means 'z->str', but here
 and below we drop 'z->' in comments.
 */
bool strtokenizer::cons(stemmer* z, int i) {
    char* b = z->str;
    switch (b[i]) {
        case 'a': case 'e': case 'i': case 'o': case 'u': return false;
        case 'y': return (i == 0) ? true : !cons(z, i - 1);
        default: return true;
    }
}

/* m(z) measures the number of consonant sequences between 0 and j. if c is
 a consonant sequence and v a vowel sequence, and <..> indicates arbitrary
 presence,
 
 <c><v>       gives 0
 <c>vc<v>     gives 1
 <c>vcvc<v>   gives 2
 <c>vcvcvc<v> gives 3
 ....
 */

int strtokenizer::m(stemmer* z) {
    int n = 0;
    int i = 0;
    int j = z->j;
    while(true)
    {  if (i > j) return n;
        if (! cons(z, i)) break; i++;
    }
    i++;
    while(true)
    {  while(true)
    {  if (i > j) return n;
        if (cons(z, i)) break;
        i++;
    }
        i++;
        n++;
        while(true)
        {  if (i > j) return n;
            if (! cons(z, i)) break;
            i++;
        }
        i++;
    }
}

/* vowelinstem(z) is true <=> 0,...j contains a vowel */

bool strtokenizer::vowelinstem(stemmer* z)
{
    int j = z->j;
    int i; for (i = 0; i <= j; i++) if (! cons(z, i)) return true;
    return false;
}

/* doublec(z, j) is true <=> j,(j-1) contain a double consonant. */

bool strtokenizer::doublec(stemmer* z, int j)
{
    char* b = z->str;
    if (j < 1) return false;
    if (b[j] != b[j - 1]) return false;
    return cons(z, j);
}

/* cvc(z, i) is true <=> i-2,i-1,i has the form consonant - vowel - consonant
 and also if the second c is not w,x or y. this is used when trying to
 restore an e at the end of a short word. e.g.
 
 cav(e), lov(e), hop(e), crim(e), but
 snow, box, tray.
 
 */

bool strtokenizer::cvc(stemmer * z, int i) {
    if (i < 2 || !cons(z, i) || cons(z, i - 1) || !cons(z, i - 2)) return false;
    char ch = z->str[i];
    if (ch  == 'w' || ch == 'x' || ch == 'y') return false;
    return true;
}

/* ends(z, s) is true <=> 0,...k ends with the string s. */

bool strtokenizer::ends(stemmer * z, const char * s) {  
    int length = s[0];
    char * b = z->str;
    int k = z->k;
    if (s[length] != b[k]) return false; /* tiny speed-up */
    if (length > k + 1) return false;
    if (memcmp(b + k - length + 1, s + 1, length) != 0) return false;
    z->j = k-length;
    return true;
}

/* setto(z, s) sets (j+1),...k to the characters in the string s, readjusting
 k. */

void strtokenizer::setto(stemmer * z, const char * s) {
    int length = s[0];
    int j = z->j;
    char * b = z->str;
    memmove(b + j + 1, s + 1, length);
    z->k = j+length;
}

/* r(z, s) is used further down. */

void strtokenizer::r(stemmer * z, const char * s) { if (m(z) > 0) setto(z, s); }

/* step1ab(z) gets rid of plurals and -ed or -ing. e.g.
 
 caresses  ->  caress
 ponies    ->  poni
 ties      ->  ti
 caress    ->  caress
 cats      ->  cat
 
 feed      ->  feed
 agreed    ->  agree
 disabled  ->  disable
 
 matting   ->  mat
 mating    ->  mate
 meeting   ->  meet
 milling   ->  mill
 messing   ->  mess
 
 meetings  ->  meet
 
 */

void strtokenizer::step1ab(stemmer * z) {
    char * b = z->str;
    if (b[z->k] == 's')
    {  if (ends(z, "\04" "sses")) z->k -= 2; else
        if (ends(z, "\03" "ies")) setto(z, "\01" "i"); else
            if (b[z->k - 1] != 's') z->k--;
    }
    if (ends(z, "\03" "eed")) { if (m(z) > 0) z->k--; } else
        if ((ends(z, "\02" "ed") || ends(z, "\03" "ing")) && vowelinstem(z))
        {  z->k = z->j;
            if (ends(z, "\02" "at")) setto(z, "\03" "ate"); else
                if (ends(z, "\02" "bl")) setto(z, "\03" "ble"); else
                    if (ends(z, "\02" "iz")) setto(z, "\03" "ize"); else
                        if (doublec(z, z->k)) {
                            z->k--;
                            int ch = b[z->k];
                            if (ch == 'l' || ch == 's' || ch == 'z') z->k++;
                        }
                        else if (m(z) == 1 && cvc(z, z->k)) setto(z, "\01" "e");
        }
}

/* step1c(z) turns terminal y to i when there is another vowel in the stem. */

void strtokenizer::step1c(stemmer * z) {
    if (ends(z, "\01" "y") && vowelinstem(z)) z->str[z->k] = 'i';
}


/* step2(z) maps double suffices to single ones. so -ization ( = -ize plus
 -ation) maps to -ize etc. note that the string before the suffix must give
 m(z) > 0. */

void strtokenizer::step2(stemmer * z) {
    switch (z->str[z->k-1]) {
        case 'a': 
            if (ends(z, "\07" "ational")) { r(z, "\03" "ate"); break; }
            if (ends(z, "\06" "tional")) { r(z, "\04" "tion"); break; }
            break;
        case 'c': 
            if (ends(z, "\04" "enci")) { r(z, "\04" "ence"); break; }
            if (ends(z, "\04" "anci")) { r(z, "\04" "ance"); break; }
            break;
        case 'e': 
            if (ends(z, "\04" "izer")) { r(z, "\03" "ize"); break; }
            break;
        case 'l': 
            if (ends(z, "\03" "bli")) { r(z, "\03" "ble"); break; } /*-DEPARTURE-*/
            
            /* To match the published algorithm, replace this line with
             case 'l': if (ends(z, "\04" "abli")) { r(z, "\04" "able"); break; } */
            
            if (ends(z, "\04" "alli")) { r(z, "\02" "al"); break; }
            if (ends(z, "\05" "entli")) { r(z, "\03" "ent"); break; }
            if (ends(z, "\03" "eli")) { r(z, "\01" "e"); break; }
            if (ends(z, "\05" "ousli")) { r(z, "\03" "ous"); break; }
            break;
        case 'o': 
            if (ends(z, "\07" "ization")) { r(z, "\03" "ize"); break; }
            if (ends(z, "\05" "ation")) { r(z, "\03" "ate"); break; }
            if (ends(z, "\04" "ator")) { r(z, "\03" "ate"); break; }
            break;
        case 's': 
            if (ends(z, "\05" "alism")) { r(z, "\02" "al"); break; }
            if (ends(z, "\07" "iveness")) { r(z, "\03" "ive"); break; }
            if (ends(z, "\07" "fulness")) { r(z, "\03" "ful"); break; }
            if (ends(z, "\07" "ousness")) { r(z, "\03" "ous"); break; }
            break;
        case 't': 
            if (ends(z, "\05" "aliti")) { r(z, "\02" "al"); break; }
            if (ends(z, "\05" "iviti")) { r(z, "\03" "ive"); break; }
            if (ends(z, "\06" "biliti")) { r(z, "\03" "ble"); break; }
            break;
        case 'g': if (ends(z, "\04" "logi")) { r(z, "\03" "log"); break; } /*-DEPARTURE-*/
            
            /* To match the published algorithm, delete this line */
            
    } }

/* step3(z) deals with -ic-, -full, -ness etc. similar strategy to step2. */

void strtokenizer::step3(stemmer * z) {
    switch (z->str[z->k]) {
        case 'e': if (ends(z, "\05" "icate")) { r(z, "\02" "ic"); break; }
            if (ends(z, "\05" "ative")) { r(z, "\00" ""); break; }
            if (ends(z, "\05" "alize")) { r(z, "\02" "al"); break; }
            break;
        case 'i': if (ends(z, "\05" "iciti")) { r(z, "\02" "ic"); break; }
            break;
        case 'l': if (ends(z, "\04" "ical")) { r(z, "\02" "ic"); break; }
            if (ends(z, "\03" "ful")) { r(z, "\00" ""); break; }
            break;
        case 's': if (ends(z, "\04" "ness")) { r(z, "\00" ""); break; }
            break;
    } }

/* step4(z) takes off -ant, -ence etc., in context <c>vcvc<v>. */

void strtokenizer::step4(stemmer * z) {
    switch (z->str[z->k-1]) {
        case 'a': 
            if (ends(z, "\02" "al")) break; return;
        case 'c': 
            if (ends(z, "\04" "ance")) break;
            if (ends(z, "\04" "ence")) break; return;
        case 'e': 
            if (ends(z, "\02" "er")) break; return;
        case 'i': 
            if (ends(z, "\02" "ic")) break; return;
        case 'l': 
            if (ends(z, "\04" "able")) break;
            if (ends(z, "\04" "ible")) break; return;
        case 'n': 
            if (ends(z, "\03" "ant")) break;
            if (ends(z, "\05" "ement")) break;
            if (ends(z, "\04" "ment")) break;
            if (ends(z, "\03" "ent")) break; return;
        case 'o': 
            if (ends(z, "\03" "ion") && (z->str[z->j] == 's' || z->str[z->j] == 't')) break;
            if (ends(z, "\02" "ou")) break; return;
            /* takes care of -ous */
        case 's': 
            if (ends(z, "\03" "ism")) break; return;
        case 't': 
            if (ends(z, "\03" "ate")) break;
            if (ends(z, "\03" "iti")) break; return;
        case 'u': 
            if (ends(z, "\03" "ous")) break; return;
        case 'v': 
            if (ends(z, "\03" "ive")) break; return;
        case 'z': 
            if (ends(z, "\03" "ize")) break; return;
        default: return;
    }
    if (m(z) > 1) z->k = z->j;
}

/* step5(z) removes a final -e if m(z) > 1, and changes -ll to -l if
 m(z) > 1. */

void strtokenizer::step5(struct stemmer * z)
{
    char * b = z->str;
    z->j = z->k;
    if (b[z->k] == 'e') {
        int a = m(z);
        if (a > 1 || a == 1 && !cvc(z, z->k - 1)) z->k--;
    }
    if (b[z->k] == 'l' && doublec(z, z->k) && m(z) > 1) z->k--;
}

/* In stem(z, b, k), b is a char pointer, and the string to be stemmed is
 from b[0] to b[k] inclusive.  Possibly b[k+1] == '\0', but it is not
 important. The stemmer adjusts the characters b[0] ... b[k] and returns
 the new end-point of the string, k'. Stemming never increases word
 length, so 0 <= k' <= k.
 */

string strtokenizer::stem(stemmer * z, string b, int k)
{
    string str;
    assert(k >= 1); /*-DEPARTURE-*/
    z->str = (char*)b.c_str();
    z->k = k; /* copy the parameters into z */
    
    /* With this line, strings of length 1 or 2 don't go through the
     stemming process, although no mention is made of this in the
     published algorithm. Remove the line to match the published
     algorithm. */
    
    step1ab(z); step1c(z); step2(z); step3(z); step4(z); step5(z);
    str.copy(z->str,z->k);
    return str;
}

