#include "ircsupportinfo.h"

#include <utility>
#include <functional>

#include <QChar>
#include <QString>
#include <QStringRef>
#include <QStringList>
#include <QList>
#include <QHash>
#include <QVector>

#include <boost/range/algorithm/find_if.hpp>

using std::pair;
using std::make_pair;
using std::function;
using std::bind;
using namespace std::placeholders;
using boost::find_if;

QHash<QString, function<void(IrcSupportInfo*)> > IrcSupportInfo::defaults;

IrcSupportInfo::IrcSupportInfo()
{
    this->setDefaults();

    for (auto fn : defaults)
        fn(this);
}

IrcSupportInfo::~IrcSupportInfo()
{
}

void IrcSupportInfo::parseToken(const QString& token)
{
    if (token.contains(' '))
        // final "are supported by this server" or equivalent.
        return;

    int idxequal = token.indexOf('=');
    QString name = token.left(idxequal),
            value = unescapeValue(token.midRef(idxequal + 1));

    if (name[0] == '-')
        this->clearParam(name);
    else
        this->setParam(name, value);
}

void IrcSupportInfo::setParam(const QString& name, const QString& value)
{
    typedef void (IrcSupportInfo::*Handler)(const QString&);

#   pragma warning(disable: 640)    // "construction of local static object is not thread-safe"; this class is only used in one thread.
    static QHash<QString, Handler> map = {
        { "CASEMAPPING", &IrcSupportInfo::setCasemappingParam },
        { "CHANLIMIT",   &IrcSupportInfo::setChanlimitParam },
        { "CHANMODES",   &IrcSupportInfo::setChanmodesParam },
        { "CHANELLEN",   &IrcSupportInfo::setUnsignedParam<&IrcSupportInfo::channellen> },
        { "CHANTYPES",   &IrcSupportInfo::setTokensParam<&IrcSupportInfo::chantypes> },
        { "EXCEPTS",     &IrcSupportInfo::setCharParam<&IrcSupportInfo::excepts, 'e'> },
        { "IDCHAN",      &IrcSupportInfo::setIdchanParam },
        { "INVEX",       &IrcSupportInfo::setCharParam<&IrcSupportInfo::invex, 'I'> },
        { "KICKLEN",     &IrcSupportInfo::setUnsignedParam<&IrcSupportInfo::kicklen> },
        { "MAXLIST",     &IrcSupportInfo::setMaxlistParam },
        { "MODES",       &IrcSupportInfo::setUnsignedParam<&IrcSupportInfo::modes> },
        { "NETWORK",     &IrcSupportInfo::setStringParam<&IrcSupportInfo::network> },
        { "NICKLEN",     &IrcSupportInfo::setUnsignedParam<&IrcSupportInfo::nicklen> },
        { "PREFIX",      &IrcSupportInfo::setPrefixParam },
        { "SAFELIST",    &IrcSupportInfo::setBoolParam<&IrcSupportInfo::safelist> },
        { "STATUSMSG",   &IrcSupportInfo::setTokensParam<&IrcSupportInfo::statusmsg> },
        { "STD",         &IrcSupportInfo::setStringParam<&IrcSupportInfo::std> },
        { "TARGMAX",     &IrcSupportInfo::setTargmaxParam },
        { "TOPICLEN",    &IrcSupportInfo::setUnsignedParam<&IrcSupportInfo::topiclen> },
        { "MAXCHANNELS", &IrcSupportInfo::setMaxchannelsParam },
        { "MAXBANS",     &IrcSupportInfo::setMaxbansParam },
        { "WALLCHOPS",   &IrcSupportInfo::addToken<&IrcSupportInfo::statusmsg, '@'> },
        { "WALLVOICES",  &IrcSupportInfo::addToken<&IrcSupportInfo::statusmsg, '+'> },
        { "ELIST",       &IrcSupportInfo::setTokensParam<&IrcSupportInfo::elist> },
        { "CHIDLEN",     &IrcSupportInfo::setChidlenParam },
        { "SILENCE",     &IrcSupportInfo::setUnsignedParam<&IrcSupportInfo::silence> },
        { "RFC2812",     &IrcSupportInfo::setBoolParam<&IrcSupportInfo::rfc2812> },
        { "PENALTY",     &IrcSupportInfo::setBoolParam<&IrcSupportInfo::penalty> },
        { "FNC",         &IrcSupportInfo::setBoolParam<&IrcSupportInfo::fnc> },
        { "AWAYLEN",     &IrcSupportInfo::setUnsignedParam<&IrcSupportInfo::awaylen> },
        //{"NOQUIT",      &IrcSupportInfo::setBoolParam<&IrcSupportInfo::noquit>},
        { "USERIP",      &IrcSupportInfo::setBoolParam<&IrcSupportInfo::userip> },
        { "CPRIVMSG",    &IrcSupportInfo::setBoolParam<&IrcSupportInfo::cprivmsg> },
        { "CNOTICE",     &IrcSupportInfo::setBoolParam<&IrcSupportInfo::cnotice> },
        { "MAXTARGETS",  &IrcSupportInfo::setUnsignedParam<&IrcSupportInfo::maxtargets> },
        { "KNOCK",       &IrcSupportInfo::setBoolParam<&IrcSupportInfo::knock> },
        { "VCHANS",      &IrcSupportInfo::setBoolParam<&IrcSupportInfo::vchans> },
        { "WATCH",       &IrcSupportInfo::setUnsignedParam<&IrcSupportInfo::watch> },
        { "WHOX",        &IrcSupportInfo::setBoolParam<&IrcSupportInfo::whox> },
        { "CALLERID",    &IrcSupportInfo::setBoolParam<&IrcSupportInfo::callerid> }
    };

    Handler handler = map[name];
    if (handler != NULL)
        (this->*handler)(value);

    tokens.insert(name, value);
}

void IrcSupportInfo::clearParam(const QString& name)
{
    setDefaults();

    function<void(IrcSupportInfo*)> setter = defaults.value(name);
    if (setter)
        setter(this);
}

void IrcSupportInfo::setDefaults()
{
    if (!defaults.empty())
        return;

    // We could have used templates instead of bound functions like for the non-defaults, but this would
    // prevent us from using initializer-lists for collections, among other things.
    this->setDefault("CASEMAPPING", &IrcSupportInfo::casemapping, Rfc1459);
    this->setDefault("CHANLIMIT", &IrcSupportInfo::chanlimit);
    defaults.insert("CHANMODES", bind(&IrcSupportInfo::clearChanmodes, _1));
    this->setDefault("CHANELLEN", &IrcSupportInfo::channellen, 200U);
    this->setDefault<QSet<QChar> >("CHANTYPES", &IrcSupportInfo::chantypes, { '#', '&' });
    this->setDefault("EXCEPTS", &IrcSupportInfo::excepts);
    this->setDefault("IDCHAN", &IrcSupportInfo::idchan);
    this->setDefault("INVEX", &IrcSupportInfo::invex);
    this->setDefault("KICKLEN", &IrcSupportInfo::kicklen);
    this->setDefault("MAXLIST", &IrcSupportInfo::maxlist);
    this->setDefault("MODES", &IrcSupportInfo::modes, 3U);
    this->setDefault("NETWORK", &IrcSupportInfo::network);
    this->setDefault("NICKLEN", &IrcSupportInfo::nicklen, 9U);
    this->setDefault<QList<pair<QChar, QChar> > >("PREFIX", &IrcSupportInfo::prefix, { { 'o','@' },{ 'v','+' } });
    this->setDefault("SAFELIST", &IrcSupportInfo::safelist);
    this->setDefault("STATUSMSG", &IrcSupportInfo::statusmsg);
    this->setDefault("STD", &IrcSupportInfo::std);
    this->setDefault("TARGMAX", &IrcSupportInfo::targmax);
    this->setDefault("TOPICLEN", &IrcSupportInfo::topiclen);

    defaults.insert("WALLCHOPS", bind(&IrcSupportInfo::removeToken, _1, &IrcSupportInfo::statusmsg, '@'));
    defaults.insert("WALLVOICES", bind(&IrcSupportInfo::removeToken, _1, &IrcSupportInfo::statusmsg, '+'));
    this->setDefault("ELIST", &IrcSupportInfo::elist);
    defaults.insert("CHIDLEN", bind(&IrcSupportInfo::clearChidlen, _1));
    this->setDefault("SILENCE", &IrcSupportInfo::silence);
    this->setDefault("RFC2812", &IrcSupportInfo::rfc2812);
    this->setDefault("PENALTY", &IrcSupportInfo::penalty);
    this->setDefault("FNC", &IrcSupportInfo::fnc);
    this->setDefault("AWAYLEN", &IrcSupportInfo::awaylen);
    this->setDefault("USERIP", &IrcSupportInfo::userip);
    this->setDefault("CPRIVMSG", &IrcSupportInfo::cprivmsg);
    this->setDefault("CNOTICE", &IrcSupportInfo::cnotice);
    this->setDefault("MAXTARGETS", &IrcSupportInfo::maxtargets, 1U);
    this->setDefault("KNOCK", &IrcSupportInfo::knock);
    this->setDefault("VCHANS", &IrcSupportInfo::vchans);
    this->setDefault("WATCH", &IrcSupportInfo::watch);
    this->setDefault("WHOX", &IrcSupportInfo::whox);
    this->setDefault("CALLERID", &IrcSupportInfo::callerid);
}

template <typename ParamType>
void IrcSupportInfo::setDefault(const QString& name, ParamType IrcSupportInfo::* param, const ParamType& dflt)
{
    defaults.insert(name, bind(&IrcSupportInfo::doClearParam<ParamType>, _1, param, dflt));
}

template <typename ParamType>
void IrcSupportInfo::doClearParam(ParamType IrcSupportInfo::* param, const ParamType& dflt)
{
    this->*param = dflt;
}

void IrcSupportInfo::clearChanmodes()
{
    this->chanmodesA.clear();
    this->chanmodesB.clear();
    this->chanmodesC.clear();
    this->chanmodesD.clear();
}

void IrcSupportInfo::removeToken(QSet<QChar> IrcSupportInfo::* var, QChar c)
{
    (this->*var).remove(c);
}

void IrcSupportInfo::clearChidlen()
{
    this->idchan.remove('!');
}

template <bool IrcSupportInfo::* var>
void IrcSupportInfo::setBoolParam(const QString& unused)
{
    this->*var = true;
    Q_UNUSED(unused);
}

template <QChar IrcSupportInfo::* var, char dflt>
void IrcSupportInfo::setCharParam(const QString& value)
{
    if (value.isEmpty())
        this->*var = dflt;
    else
        this->*var = value.at(0);
}

template <uint IrcSupportInfo::* var>
void IrcSupportInfo::setUnsignedParam(const QString& value)
{
    this->*var = value.toUInt();
}

template <QString IrcSupportInfo::* var>
void IrcSupportInfo::setStringParam(const QString& value)
{
    this->*var = value;
}

void IrcSupportInfo::setTokensParam(QSet<QChar> IrcSupportInfo::* param, const QString& value)
{
    (this->*param).clear();
    for (QChar c : value)
        (this->*param).insert(c);
}

template <QSet<QChar> IrcSupportInfo::* var>
void IrcSupportInfo::setTokensParam(const QString& value)
{
    this->setTokensParam(var, value);
}

template <QSet<QChar> IrcSupportInfo::* var, char c>
void IrcSupportInfo::addToken(const QString& unused)
{
    (this->*var).insert(c);
    Q_UNUSED(unused);
}

void IrcSupportInfo::setPrefixParam(const QString& value)
{
    if (value.isEmpty() || value[0] != '(') return;

    int delim = value.indexOf(')', 1);
    QStringRef letters = value.midRef(1, delim - 1),
               symbols = value.midRef(delim + 1);

    for (QStringRef::const_iterator letter = letters.cbegin(), symbol = symbols.cbegin(),
                                    endl = letters.cend(), ends = symbols.cend();
         letter != endl && symbol != ends;
         ++letter, ++symbol)
    {
        this->prefix.append(make_pair(*letter, *symbol));
    }
}

void IrcSupportInfo::setChanmodesParam(const QString& value)
{
    QStringList parts = value.split(',');
    auto item = parts.cbegin(), end = parts.cend();
    QSet<QChar> IrcSupportInfo::* sets[] = {
        &IrcSupportInfo::chanmodesA, 
        &IrcSupportInfo::chanmodesB, 
        &IrcSupportInfo::chanmodesC, 
        &IrcSupportInfo::chanmodesD 
    };

    for (auto var : sets)
    {
        if (item == end) return;    // fewer than four definitions
        this->setTokensParam(var, *item++);
    }
}

void IrcSupportInfo::setChanlimitParam(const QString& value)
{
    this->chanlimit.clear();

    auto list = parsePairList(value);

    for (auto item : list)
        for (QChar symbol : item.first)
            chanlimit.insert(symbol, item.second);
}

void IrcSupportInfo::setMaxchannelsParam(const QString& value)
{
    this->chanlimit.clear();

    bool ok;
    int limit = value.toInt(&ok);
    if (!ok) return;

    for (QChar prefix : this->chantypes)
        this->chanlimit.insert(prefix, limit);
}

void IrcSupportInfo::setMaxlistParam(const QString& value)
{
    this->maxlist = parsePairList(value);
}

void IrcSupportInfo::setMaxbansParam(const QString& value)
{   // this isn't meant to be efficient, as MAXBANS is deprecated anyway.
    bool ok;
    int limit = value.toInt(&ok);
    if (!ok) return;

    // find an item in MAXLIST containing 'b' (ban limit)
    auto it = find_if(
        this->maxlist,
        [](pair<QString, int>& item) { return item.first.indexOf('b') != -1; }
    );

    if (it != this->maxlist.end())
    {   // found one.
        if (it->second == limit) return;    // no change.
        if (it->first == "b")
        {   // limit isn't shared with others. good. just change it.
            it->second = limit;
            return;
        }
        else
        {
            // oh, fun, we have to split it.
            it->first.remove(it->first.indexOf('b'));
        }
    }

    // most likely case, as MAXLIST and MAXBANS shouldn't coexist.
    this->maxlist.append(make_pair(QString("b"), limit));
}

void IrcSupportInfo::setIdchanParam(const QString& value)
{
    auto list = parsePairList(value);

    for (auto item : list)
        for (QChar symbol : item.first)
            this->idchan.insert(symbol, item.second);
}

void IrcSupportInfo::setChidlenParam(const QString& value)
{
    this->idchan['!'] = value.toUInt();
}

void IrcSupportInfo::setTargmaxParam(const QString& value)
{
    auto list = parsePairList(value);
    for (auto item : list)
        this->targmax.insert(item.first, item.second);
}

QList<pair<QString, int> > IrcSupportInfo::parsePairList(const QString& value)
{
    QList<pair<QString, int> > parsed;

    for (auto item : value.splitRef(','))
    {
        int colon = item.indexOf(':');
        if (colon == -1) continue;  // missing ':'
        auto left = item.mid(0, colon);
        auto right = item.mid(colon + 1);

        int number;
        if (right.isEmpty())
            number = -1;
        else
        {
            bool ok;
            number = right.toInt(&ok);
            if (!ok) continue;  // couldn't parse number.
        }

        parsed.append(make_pair(left.toString(), number));
    }

    return parsed;
}

void IrcSupportInfo::setCasemappingParam(const QString& value)
{
    if (value == "ascii")
        casemapping = Ascii;
    else if (value == "strict-rfc1459")
        casemapping = StrictRfc1459;
    else
        casemapping = Rfc1459;
}

QString IrcSupportInfo::unescapeValue(const QStringRef& range)
{
    QString unescaped;
    for (auto it = range.begin(), end = range.end(); it != end; qt_noop())
    {
        QChar c = *it;

        if (c == '\\') c = unescapeChar(it, end);
        else ++it;

        unescaped.append(c);
    }

    return unescaped;
}

QChar IrcSupportInfo::unescapeChar(QString::const_iterator& it, const QString::const_iterator& end)
{
    QString::const_iterator escape = ++it;
    if (escape == end || *escape++ != 'x') return '\\';

    QString hex (escape, 2);
    bool ok;
    ushort ucs = hex.toUShort(&ok, 16);
    if (!ok)
        return '\\';
    else
    {
        it = escape + 2;
        return QChar(ucs);
    }
}
