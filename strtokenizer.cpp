#include "strtokenizer.h"

using namespace std;

void strtokenizer::strtokenizer_operate(string str, string seperators, bool preEnable) {
    z = NULL;
    if (preEnable == true) {
        if (stopListRead == false) {
            read_stop_list();
        }
        preprocess(str);
    } else {
        parse(str, seperators);
    }
}

void strtokenizer::preprocess(string text) {
    text = verify(text);
    regex re1("([a-z])([0-9])");
    regex re2("([0-9])([a-z])");
    regex re3("\\s+");
    //text = regex_replace(text, re1, "\\1 \\2");
    //text = regex_replace(text, re2, "\\1 \\2");
    //text = regex_replace(text, re3, " ");
    if (text.size() > 2)
    {
        text = stem(text);
    }
    text = stopword_remover(text);
    if (text != "")
    {
        tokens.push_back(text);
    }
    
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
    if (!fp.is_open())
    {
        cout << "can't open stop list" << endl;
    }
    for (int i = (int)'a'; i <= (int)'z'; ++i) {
        tmp = toascii(i);
        stopList.push_back(tmp.substr(0,1));
        
    }
    getline(fp,str);
    getline(fp,str);
    while (1) {
        getline(fp, str);
        if (fp.eof())
        {
            break;
        }
        str = str.substr(5,str.size()-3-5);
        str.erase(find(str.begin(), str.end(), ']'));
        transform(str.begin(),str.end(),str.begin(),::tolower);
        stopList.push_back(str);
    }
    stopListRead = true;
}

string strtokenizer::stopword_remover(string str) {
    for (vector<string>::size_type i = 0; i < stopList.size(); i++) {
        if (stopList[i].compare(str) == 0) {
            return "";
        }
    }
    return str;
}

string strtokenizer::verify(string str) {
    for (string::size_type i = 0; i < str.size(); ++i) {
        if (toascii(str[i]) > 127) {
            str.erase(i);
        } else if (isalpha(str[i])) {
            str[i] = tolower(str[i]);
            continue;
        } else {
            str.erase(i);
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

/* cons(i) is true <=> b[i] is a consonant. ('b' means 'z->str', but here
 and below we drop 'z->' in comments.
 */
bool strtokenizer::cons(int i) {
    string b = z->str;
    switch (b[i]) {
        case 'a': case 'e': case 'i': case 'o': case 'u': return false;
        case 'y': 
            if (i == 0) {return true;}
            else {return !cons(i - 1);}
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

int strtokenizer::m() {
    int n = 0;
    int i = 0;
    int j = z->j;
    while(true) {  
        if (i > j) return n;
        if (!cons(i)) break; 
        i++;
    }
    i++;
    while(true) {  
        while(true) {  
            if (i > j) return n;
            if (cons(i)) break;
            i++;
        }
        i++;
        n++;
        while(true) {  
            if (i > j) return n;
            if (! cons(i)) break;
            i++;
        }
        i++;
    }
}

/* vowelinstem(z) is true <=> 0,...j contains a vowel */

bool strtokenizer::vowelinstem()
{
    int j = z->j;
    int i; 
    for (i = 0; i <= j; i++) {
        if (!cons(i)) return true;
    }
    return false;
}

/* doublec(j) is true <=> j,(j-1) contain a double consonant. */

bool strtokenizer::doublec(int j) {
    if (j < 1) return false;
    if (z->str[j] != z->str[j - 1]) return false;
    return cons(j);
}

/* cvc(i) is true <=> i-2,i-1,i has the form consonant - vowel - consonant
 and also if the second c is not w,x or y. this is used when trying to
 restore an e at the end of a short word. e.g.
 
 cav(e), lov(e), hop(e), crim(e), but
 snow, box, tray.
 
 */

bool strtokenizer::cvc(int i) {
    if (i < 2 || !cons(i) || cons(i - 1) || !cons(i - 2)) return false;
    char ch = z->str[i];
    if (ch  == 'w' || ch == 'x' || ch == 'y') return false;
    return true;
}

/* ends(s) is true <=> 0,...k ends with the string s. */

bool strtokenizer::ends(string s) { 
    int length = s.size(); 
    int k = (z->str).size();

    if (s.back() != (z->str).back()) return false; /* tiny speed-up */
    if (s.size() > (z->str).size()) return false;
    if ((z->str).substr(k-length,length).compare(s) != 0) return false;
    //if (memcmp(b + k - length + 1, s + 1, length) != 0) return false;
    z->j = k-length-1;
    return true;
}

/* setto(s) sets (j+1),...k to the characters in the string s, readjusting
 k. */

void strtokenizer::setto(string s) {
    int j = z->j;
    cout << z->str << " ";
    z->str = (z->str).substr(0, j+1).append(s);
    cout << z->str << endl;
    //memmove(b + j + 1, s + 1, length);
}

/* r(s) is used further down. */

void strtokenizer::r(string s) { if (m() > 0) setto(s); }

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

void strtokenizer::step1ab() {
    if ((z->str).back() == 's') {  
        if (ends("sses")) (z->str).erase((z->str).size()-2,2); 
        else if (ends("ies")) setto("i"); 
        else if ((z->str)[(z->str).size()-2] != 's') (z->str).erase((z->str).size()-1,1);
    }
    if (ends("eed")) { 
        if (m() > 0) (z->str).erase((z->str).size()-1,1); 
    } 
    else if ((ends("ed") || ends("ing")) && vowelinstem()) {  
        z->str = (z->str).substr(0,z->j+1);
        if (ends("at")) setto("ate"); 
        else if (ends("bl")) setto("ble"); 
        else if (ends("iz")) setto("ize"); 
        else if (doublec((z->str).size()-1)) {
            int ch = (z->str).back();
            if (ch == 'l' || ch == 's' || ch == 'z') {}
            else {(z->str).erase((z->str).size()-1,1);}
        }
        else if (m() == 1 && cvc((z->str).size()-1)) setto("e");
    }
}

/* step1c(z) turns terminal y to i when there is another vowel in the stem. */

void strtokenizer::step1c() {
    if (ends("y") && vowelinstem()) z->str[(z->str).size()-1] = 'i';
}


/* step2(z) maps double suffices to single ones. so -ization ( = -ize plus
 -ation) maps to -ize etc. note that the string before the suffix must give
 m(z) > 0. */

void strtokenizer::step2() {
    switch (z->str[(z->str).size()-2]) {
        case 'a': 
            if (ends("ational")) { r("ate"); break; }
            if (ends("tional")) { r("tion"); break; }
            break;
        case 'c': 
            if (ends("enci")) { r("ence"); break; }
            if (ends("anci")) { r("ance"); break; }
            break;
        case 'e': 
            if (ends("izer")) { r("ize"); break; }
            break;
        case 'l': 
            if (ends("bli")) { r("ble"); break; } /*-DEPARTURE-*/
            
            /* To match the published algorithm, replace this line with
             case 'l': if (ends("abli")) { r("able"); break; } */
            
            if (ends("alli")) { r("al"); break; }
            if (ends("entli")) { r("ent"); break; }
            if (ends("eli")) { r("e"); break; }
            if (ends("ousli")) { r("ous"); break; }
            break;
        case 'o': 
            if (ends("ization")) { r("ize"); break; }
            if (ends("ation")) { r("ate"); break; }
            if (ends("ator")) { r("ate"); break; }
            break;
        case 's': 
            if (ends("alism")) { r("al"); break; }
            if (ends("iveness")) { r("ive"); break; }
            if (ends("fulness")) { r("ful"); break; }
            if (ends("ousness")) { r("ous"); break; }
            break;
        case 't': 
            if (ends("aliti")) { r("al"); break; }
            if (ends("iviti")) { r("ive"); break; }
            if (ends("biliti")) { r("ble"); break; }
            break;
        case 'g': if (ends("logi")) { r("log"); break; } /*-DEPARTURE-*/
            
            /* To match the published algorithm, delete this line */
            
    } }

/* step3(z) deals with -ic-, -full, -ness etc. similar strategy to step2. */

void strtokenizer::step3() {
    switch ((z->str).back()) {
        case 'e': 
            if (ends("icate")) { r("ic"); break; }
            if (ends("ative")) { r(""); break; }
            if (ends("alize")) { r("al"); break; }
            break;
        case 'i': 
            if (ends("iciti")) { r("ic"); break; }
            break;
        case 'l': 
            if (ends("ical")) { r("ic"); break; }
            if (ends("ful")) { r(""); break; }
            break;
        case 's': 
            if (ends("ness")) { r(""); break; }
            break;
    } 
}

/* step4(z) takes off -ant, -ence etc., in context <c>vcvc<v>. */

void strtokenizer::step4() {
    switch (z->str[(z->str).size()-2]) {
        case 'a': 
            if (ends("al")) break; return;
        case 'c': 
            if (ends("ance")) break;
            if (ends("ence")) break; return;
        case 'e': 
            if (ends("er")) break; return;
        case 'i': 
            if (ends("ic")) break; return;
        case 'l': 
            if (ends("able")) break;
            if (ends("ible")) break; return;
        case 'n': 
            if (ends("ant")) break;
            if (ends("ement")) break;
            if (ends("ment")) break;
            if (ends("ent")) break; return;
        case 'o': 
            if (ends("ion") && (z->str[z->j] == 's' || z->str[z->j] == 't')) break;
            if (ends("ou")) break; return;
            /* takes care of -ous */
        case 's': 
            if (ends("ism")) break; return;
        case 't': 
            if (ends("ate")) break;
            if (ends("iti")) break; return;
        case 'u': 
            if (ends("ous")) break; return;
        case 'v': 
            if (ends("ive")) break; return;
        case 'z': 
            if (ends("ize")) break; return;
        default: return;
    }
    if (m() > 1) {z->str = (z->str).substr(0,z->j+1);}
}

/* step5(z) removes a final -e if m(z) > 1, and changes -ll to -l if
 m(z) > 1. */

void strtokenizer::step5() {
    z->j = (z->str).size()-1;
    if ((z->str).back() == 'e') {
        int a = m();
        if (a > 1 || (a == 1 && !cvc((z->str).size()-2))) (z->str).erase((z->str).size()-1,1);
    }
    if ((z->str).back() == 'l' && doublec((z->str).size()-1) && m() > 1) (z->str).erase((z->str).size()-1,1);
}

/* In stem(b, k), b is a char pointer, and the string to be stemmed is
 from b[0] to b[k] inclusive.  Possibly b[k+1] == '\0', but it is not
 important. The stemmer adjusts the characters b[0] ... b[k] and returns
 the new end-point of the string, k'. Stemming never increases word
 length, so 0 <= k' <= k.
 */

string strtokenizer::stem(string b) {
    string str;
    if (z == NULL)
    {
        z = new stemmer(b);
    } else {
        z->str = b;
    }

    step1ab(); step1c(); step2(); step3(); step4(); step5();
    //str.copy(z->str,z->k);
    return z->str;
}

