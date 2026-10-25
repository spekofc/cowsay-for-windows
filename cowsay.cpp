// cowsay for Windows - arquivo unico, C++17
// MSVC:  cl /EHsc /utf-8 /O2 cowsay.cpp
// MinGW: g++ -O2 -std=c++17 -static cowsay.cpp -o cowsay.exe -lshell32

#include <windows.h>
#include <shellapi.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifdef _MSC_VER
#pragma comment(lib, "shell32.lib")
#endif

using std::string;
using std::vector;

// ---------- utilitarios UTF-8 ----------

static string toUtf8(const wchar_t* w) {
    int n = WideCharToMultiByte(CP_UTF8, 0, w, -1, nullptr, 0, nullptr, nullptr);
    if (n <= 1) return "";
    string s(n - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w, -1, &s[0], n, nullptr, nullptr);
    return s;
}

// comprimento em code points
static size_t ulen(const string& s) {
    size_t n = 0;
    for (unsigned char c : s)
        if ((c & 0xC0) != 0x80) ++n;
    return n;
}

// pega os primeiros `count` code points
static string utake(const string& s, size_t count) {
    size_t n = 0, i = 0;
    for (; i < s.size(); ++i) {
        if (((unsigned char)s[i] & 0xC0) != 0x80) {
            if (n == count) break;
            ++n;
        }
    }
    return s.substr(0, i);
}

// ---------- quebra de linha ----------

static vector<string> wrapLine(const string& line, size_t width) {
    vector<string> out;
    std::istringstream iss(line);
    string word, cur;
    while (iss >> word) {
        // palavras maiores que a largura sao cortadas
        while (ulen(word) > width) {
            if (!cur.empty()) { out.push_back(cur); cur.clear(); }
            string part = utake(word, width);
            out.push_back(part);
            word = word.substr(part.size());
        }
        if (cur.empty()) cur = word;
        else if (ulen(cur) + 1 + ulen(word) <= width) cur += " " + word;
        else { out.push_back(cur); cur = word; }
    }
    if (!cur.empty() || out.empty()) out.push_back(cur);
    return out;
}

static vector<string> buildLines(const string& text, size_t width, bool wrap) {
    vector<string> lines;
    std::istringstream iss(text);
    string line;
    while (std::getline(iss, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        // tab -> 4 espacos
        string t;
        for (char c : line) t += (c == '\t') ? string("    ") : string(1, c);
        if (wrap) {
            auto w = wrapLine(t, width);
            lines.insert(lines.end(), w.begin(), w.end());
        } else {
            lines.push_back(t);
        }
    }
    if (lines.empty()) lines.push_back("");
    return lines;
}

// ---------- balao ----------

static string balloon(const vector<string>& lines, bool think) {
    size_t max = 0;
    for (auto& l : lines) max = std::max(max, ulen(l));

    string out = " " + string(max + 2, '_') + "\n";
    size_t n = lines.size();
    for (size_t i = 0; i < n; ++i) {
        string l = lines[i] + string(max - ulen(lines[i]), ' ');
        string L, R;
        if (think)          { L = "("; R = ")"; }
        else if (n == 1)    { L = "<"; R = ">"; }
        else if (i == 0)    { L = "/"; R = "\\"; }
        else if (i == n - 1){ L = "\\"; R = "/"; }
        else                { L = "|"; R = "|"; }
        out += L + " " + l + " " + R + "\n";
    }
    out += " " + string(max + 2, '-') + "\n";
    return out;
}

// ---------- vacas ----------

static string cowTemplate(const string& name, bool& found) {
    found = true;
    if (name == "default")
        return R"(        $t   ^__^
         $t  ($e)\_______
            (__)\       )\/\
             $T ||----w |
                ||     ||
)";
    if (name == "tux")
        return R"(   $t
    $t
        .--.
       |$e |
       |:_/ |
      //   \ \
     (|     | )
    /'\_   _/`\
    \___)=(___/
)";
    if (name == "small")
        return R"(  $t  ^__^
   $t ($e)\_______
      (__)\       )\/\
       $T ||----w |
          ||     ||
)";
    found = false;
    return "";
}

static string replaceAll(string s, const string& from, const string& to) {
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != string::npos) {
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
    return s;
}

static void usage() {
    std::cout <<
R"(cowsay for Windows

Uso: cowsay [opcoes] [mensagem]
     echo ola | cowsay

Opcoes:
  -e <olhos>    define os olhos (2 caracteres)   -T <lingua>  define a lingua
  -f <vaca>     default, tux, small             -W <n>       largura (padrao 40)
  -n            nao quebra linhas                --think      modo pensamento
  -b borg  -d morta  -g ganancioso  -p paranoica  -s chapada
  -t cansada  -w ligada  -y jovem
  -l            lista as vacas
  -h            ajuda
)";
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    int argc = 0;
    LPWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    vector<string> args;
    for (int i = 1; i < argc; ++i) args.push_back(toUtf8(wargv[i]));
    LocalFree(wargv);

    string eyes = "oo", tongue = "  ", cow = "default";
    size_t width = 40;
    bool wrap = true, think = false;
    vector<string> words;

    for (size_t i = 0; i < args.size(); ++i) {
        const string& a = args[i];
        auto next = [&](string& dst) {
            if (i + 1 < args.size()) dst = args[++i];
        };
        if (a == "-h" || a == "--help") { usage(); return 0; }
        else if (a == "-l") { std::cout << "default\ntux\nsmall\n"; return 0; }
        else if (a == "-n") wrap = false;
        else if (a == "--think") think = true;
        else if (a == "-e") { next(eyes); }
        else if (a == "-T") { next(tongue); }
        else if (a == "-f") { next(cow); }
        else if (a == "-W") {
            string w; next(w);
            try { width = std::max<size_t>(1, std::stoul(w)); } catch (...) {}
        }
        else if (a == "-b") eyes = "==";
        else if (a == "-d") { eyes = "xx"; tongue = "U "; }
        else if (a == "-g") eyes = "$$";
        else if (a == "-p") eyes = "@@";
        else if (a == "-s") { eyes = "**"; tongue = "U "; }
        else if (a == "-t") eyes = "--";
        else if (a == "-w") eyes = "OO";
        else if (a == "-y") eyes = "..";
        else words.push_back(a);
    }

    // olhos e lingua sempre com 2 code points
    eyes = utake(eyes + "  ", 2);
    tongue = utake(tongue + "  ", 2);

    string text;
    if (!words.empty()) {
        for (size_t i = 0; i < words.size(); ++i) text += (i ? " " : "") + words[i];
    } else {
        HANDLE hin = GetStdHandle(STD_INPUT_HANDLE);
        if (GetFileType(hin) == FILE_TYPE_CHAR) {   // teclado, sem pipe
            usage();
            return 1;
        }
        std::ostringstream ss;
        ss << std::cin.rdbuf();
        text = ss.str();
    }

    bool found;
    string tpl = cowTemplate(cow, found);
    if (!found) {
        std::cerr << "cowsay: vaca '" << cow << "' nao encontrada (use -l)\n";
        return 1;
    }

    tpl = replaceAll(tpl, "$t", think ? "o" : "\\");
    tpl = replaceAll(tpl, "$e", eyes);
    tpl = replaceAll(tpl, "$T", tongue);

    std::cout << balloon(buildLines(text, width, wrap), think) << tpl;
    return 0;
}