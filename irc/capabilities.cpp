#include "capabilities.h"

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

namespace Irc
{

QHash<QString, function<void(Capabilities*)> > Capabilities::defaults;

Capabilities::Capabilities()
{
    this->setDefaults();

    for (auto fn : defaults)
        fn(this);
}

Capabilities::~Capabilities()
{
}

const QString& Capabilities::networkName() const
{
    return this->network;
}


void Capabilities::parseToken(const QString& token)
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

void Capabilities::setParam(const QString& name, const QString& value)
{
    typedef void (Capabilities::*Handler)(const QString&);

#   pragma warning(disable: 640)    // "construction of local static object is not thread-safe"; this class is only used in one thread.
    static QHash<QString, Handler> map = {
        { "CASEMAPPING", &Capabilities::setCasemappingParam },
        { "CHANLIMIT",   &Capabilities::setChanlimitParam },
        { "CHANMODES",   &Capabilities::setChanmodesParam },
        { "CHANELLEN",   &Capabilities::setUnsignedParam<&Capabilities::channellen> },
        { "CHANTYPES",   &Capabilities::setTokensParam<&Capabilities::chantypes> },
        { "EXCEPTS",     &Capabilities::setCharParam<&Capabilities::excepts, 'e'> },
        { "IDCHAN",      &Capabilities::setIdchanParam },
        { "INVEX",       &Capabilities::setCharParam<&Capabilities::invex, 'I'> },
        { "KICKLEN",     &Capabilities::setUnsignedParam<&Capabilities::kicklen> },
        { "MAXLIST",     &Capabilities::setMaxlistParam },
        { "MODES",       &Capabilities::setUnsignedParam<&Capabilities::modes> },
        { "NETWORK",     &Capabilities::setStringParam<&Capabilities::network> },
        { "NICKLEN",     &Capabilities::setUnsignedParam<&Capabilities::nicklen> },
        { "PREFIX",      &Capabilities::setPrefixParam },
        { "SAFELIST",    &Capabilities::setBoolParam<&Capabilities::safelist> },
        { "STATUSMSG",   &Capabilities::setTokensParam<&Capabilities::statusmsg> },
        { "STD",         &Capabilities::setStringParam<&Capabilities::std> },
        { "TARGMAX",     &Capabilities::setTargmaxParam },
        { "TOPICLEN",    &Capabilities::setUnsignedParam<&Capabilities::topiclen> },
        { "MAXCHANNELS", &Capabilities::setMaxchannelsParam },
        { "MAXBANS",     &Capabilities::setMaxbansParam },
        { "WALLCHOPS",   &Capabilities::addToken<&Capabilities::statusmsg, '@'> },
        { "WALLVOICES",  &Capabilities::addToken<&Capabilities::statusmsg, '+'> },
        { "ELIST",       &Capabilities::setTokensParam<&Capabilities::elist> },
        { "CHIDLEN",     &Capabilities::setChidlenParam },
        { "SILENCE",     &Capabilities::setUnsignedParam<&Capabilities::silence> },
        { "RFC2812",     &Capabilities::setBoolParam<&Capabilities::rfc2812> },
        { "PENALTY",     &Capabilities::setBoolParam<&Capabilities::penalty> },
        { "FNC",         &Capabilities::setBoolParam<&Capabilities::fnc> },
        { "AWAYLEN",     &Capabilities::setUnsignedParam<&Capabilities::awaylen> },
        //{"NOQUIT",      &SupportInfo::setBoolParam<&SupportInfo::noquit>},
        { "USERIP",      &Capabilities::setBoolParam<&Capabilities::userip> },
        { "CPRIVMSG",    &Capabilities::setBoolParam<&Capabilities::cprivmsg> },
        { "CNOTICE",     &Capabilities::setBoolParam<&Capabilities::cnotice> },
        { "MAXTARGETS",  &Capabilities::setUnsignedParam<&Capabilities::maxtargets> },
        { "KNOCK",       &Capabilities::setBoolParam<&Capabilities::knock> },
        { "VCHANS",      &Capabilities::setBoolParam<&Capabilities::vchans> },
        { "WATCH",       &Capabilities::setUnsignedParam<&Capabilities::watch> },
        { "WHOX",        &Capabilities::setBoolParam<&Capabilities::whox> },
        { "CALLERID",    &Capabilities::setBoolParam<&Capabilities::callerid> }
    };

    Handler handler = map[name];
    if (handler != NULL)
        (this->*handler)(value);

    tokens.insert(name, value);
}

void Capabilities::clearParam(const QString& name)
{
    this->setDefaults();

    function<void(Capabilities*)> setter = defaults.value(name);
    if (setter)
        setter(this);
}

void Capabilities::setDefaults()
{
    if (!defaults.empty())
        return;

    // We could have used templates instead of bound functions like for the non-defaults, but this would
    // prevent us from using initializer-lists for collections, among other things.
    this->setDefault("CASEMAPPING", &Capabilities::casemapping, Rfc1459);
    this->setDefault("CHANLIMIT", &Capabilities::chanlimit);
    defaults.insert("CHANMODES", bind(&Capabilities::clearChanmodes, _1));
    this->setDefault("CHANELLEN", &Capabilities::channellen, 200U);
    this->setDefault<QSet<QChar> >("CHANTYPES", &Capabilities::chantypes, { '#', '&' });
    this->setDefault("EXCEPTS", &Capabilities::excepts);
    this->setDefault("IDCHAN", &Capabilities::idchan);
    this->setDefault("INVEX", &Capabilities::invex);
    this->setDefault("KICKLEN", &Capabilities::kicklen);
    this->setDefault("MAXLIST", &Capabilities::maxlist);
    this->setDefault("MODES", &Capabilities::modes, 3U);
    this->setDefault("NETWORK", &Capabilities::network);
    this->setDefault("NICKLEN", &Capabilities::nicklen, 9U);
    this->setDefault<QList<pair<QChar, QChar> > >("PREFIX", &Capabilities::prefix, { { 'o','@' },{ 'v','+' } });
    this->setDefault("SAFELIST", &Capabilities::safelist);
    this->setDefault("STATUSMSG", &Capabilities::statusmsg);
    this->setDefault("STD", &Capabilities::std);
    this->setDefault("TARGMAX", &Capabilities::targmax);
    this->setDefault("TOPICLEN", &Capabilities::topiclen);

    defaults.insert("WALLCHOPS", bind(&Capabilities::removeToken, _1, &Capabilities::statusmsg, '@'));
    defaults.insert("WALLVOICES", bind(&Capabilities::removeToken, _1, &Capabilities::statusmsg, '+'));
    this->setDefault("ELIST", &Capabilities::elist);
    defaults.insert("CHIDLEN", bind(&Capabilities::clearChidlen, _1));
    this->setDefault("SILENCE", &Capabilities::silence);
    this->setDefault("RFC2812", &Capabilities::rfc2812);
    this->setDefault("PENALTY", &Capabilities::penalty);
    this->setDefault("FNC", &Capabilities::fnc);
    this->setDefault("AWAYLEN", &Capabilities::awaylen);
    this->setDefault("USERIP", &Capabilities::userip);
    this->setDefault("CPRIVMSG", &Capabilities::cprivmsg);
    this->setDefault("CNOTICE", &Capabilities::cnotice);
    this->setDefault("MAXTARGETS", &Capabilities::maxtargets, 1U);
    this->setDefault("KNOCK", &Capabilities::knock);
    this->setDefault("VCHANS", &Capabilities::vchans);
    this->setDefault("WATCH", &Capabilities::watch);
    this->setDefault("WHOX", &Capabilities::whox);
    this->setDefault("CALLERID", &Capabilities::callerid);
}

template <typename ParamType>
void Capabilities::setDefault(const QString& name, ParamType Capabilities::* param, const ParamType& dflt)
{
    defaults.insert(name, bind(&Capabilities::doClearParam<ParamType>, _1, param, dflt));
}

template <typename ParamType>
void Capabilities::doClearParam(ParamType Capabilities::* param, const ParamType& dflt)
{
    this->*param = dflt;
}

void Capabilities::clearChanmodes()
{
    this->chanmodesA.clear();
    this->chanmodesB.clear();
    this->chanmodesC.clear();
    this->chanmodesD.clear();
}

void Capabilities::removeToken(QSet<QChar> Capabilities::* var, QChar c)
{
    (this->*var).remove(c);
}

void Capabilities::clearChidlen()
{
    this->idchan.remove('!');
}

template <bool Capabilities::* var>
void Capabilities::setBoolParam(const QString& unused)
{
    this->*var = true;
    Q_UNUSED(unused);
}

template <QChar Capabilities::* var, char dflt>
void Capabilities::setCharParam(const QString& value)
{
    if (value.isEmpty())
        this->*var = dflt;
    else
        this->*var = value.at(0);
}

template <uint Capabilities::* var>
void Capabilities::setUnsignedParam(const QString& value)
{
    this->*var = value.toUInt();
}

template <QString Capabilities::* var>
void Capabilities::setStringParam(const QString& value)
{
    this->*var = value;
}

void Capabilities::setTokensParam(QSet<QChar> Capabilities::* param, const QString& value)
{
    (this->*param).clear();
    for (QChar c : value)
        (this->*param).insert(c);
}

template <QSet<QChar> Capabilities::* var>
void Capabilities::setTokensParam(const QString& value)
{
    this->setTokensParam(var, value);
}

template <QSet<QChar> Capabilities::* var, char c>
void Capabilities::addToken(const QString& unused)
{
    (this->*var).insert(c);
    Q_UNUSED(unused);
}

void Capabilities::setPrefixParam(const QString& value)
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

void Capabilities::setChanmodesParam(const QString& value)
{
    QStringList parts = value.split(',');
    auto item = parts.cbegin(), end = parts.cend();
    QSet<QChar> Capabilities::* sets[] = {
        &Capabilities::chanmodesA, 
        &Capabilities::chanmodesB, 
        &Capabilities::chanmodesC, 
        &Capabilities::chanmodesD 
    };

    for (auto var : sets)
    {
        if (item == end) return;    // fewer than four definitions
        this->setTokensParam(var, *item++);
    }
}

void Capabilities::setChanlimitParam(const QString& value)
{
    this->chanlimit.clear();

    auto list = parsePairList(value);

    for (auto item : list)
        for (QChar symbol : item.first)
            chanlimit.insert(symbol, item.second);
}

void Capabilities::setMaxchannelsParam(const QString& value)
{
    this->chanlimit.clear();

    bool ok;
    int limit = value.toInt(&ok);
    if (!ok)
        return;

    for (QChar prefix : this->chantypes)
        this->chanlimit.insert(prefix, limit);
}

void Capabilities::setMaxlistParam(const QString& value)
{
    this->maxlist = parsePairList(value);
}

void Capabilities::setMaxbansParam(const QString& value)
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
        if (it->second == limit)
            return;    // no change.
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

void Capabilities::setIdchanParam(const QString& value)
{
    auto list = parsePairList(value);

    for (auto item : list)
        for (QChar symbol : item.first)
            this->idchan.insert(symbol, item.second);
}

void Capabilities::setChidlenParam(const QString& value)
{
    this->idchan['!'] = value.toUInt();
}

void Capabilities::setTargmaxParam(const QString& value)
{
    auto list = parsePairList(value);
    for (auto item : list)
        this->targmax.insert(item.first, item.second);
}

QList<pair<QString, int> > Capabilities::parsePairList(const QString& value)
{
    QList<pair<QString, int> > parsed;

    for (auto item : value.splitRef(','))
    {
        int colon = item.indexOf(':');
        if (colon == -1) 
            continue;  // missing ':'
        auto left = item.mid(0, colon);
        auto right = item.mid(colon + 1);

        int number;
        if (right.isEmpty())
            number = -1;
        else
        {
            bool ok;
            number = right.toInt(&ok);
            if (!ok)
                continue;  // couldn't parse number.
        }

        parsed.append(make_pair(left.toString(), number));
    }

    return parsed;
}

void Capabilities::setCasemappingParam(const QString& value)
{
    if (value == "ascii")
        casemapping = Ascii;
    else if (value == "strict-rfc1459")
        casemapping = StrictRfc1459;
    else
        casemapping = Rfc1459;
}

QString Capabilities::unescapeValue(const QStringRef& range)
{
    QString unescaped;
    for (auto it = range.begin(), end = range.end(); it != end; qt_noop())
    {
        QChar c = *it;

        if (c == '\\')
            c = unescapeChar(it, end);
        else
            ++it;

        unescaped.append(c);
    }

    return unescaped;
}

QChar Capabilities::unescapeChar(QString::const_iterator& it, const QString::const_iterator& end)
{
    QString::const_iterator escape = ++it;
    if (escape == end || *escape++ != 'x')
        return '\\';

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

}
