#include "supportinfo.h"

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

QHash<QString, function<void(SupportInfo*)> > SupportInfo::defaults;

SupportInfo::SupportInfo()
{
    this->setDefaults();

    for (auto fn : defaults)
        fn(this);
}

SupportInfo::~SupportInfo()
{
}

void SupportInfo::parseToken(const QString& token)
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

void SupportInfo::setParam(const QString& name, const QString& value)
{
    typedef void (SupportInfo::*Handler)(const QString&);

#   pragma warning(disable: 640)    // "construction of local static object is not thread-safe"; this class is only used in one thread.
    static QHash<QString, Handler> map = {
        { "CASEMAPPING", &SupportInfo::setCasemappingParam },
        { "CHANLIMIT",   &SupportInfo::setChanlimitParam },
        { "CHANMODES",   &SupportInfo::setChanmodesParam },
        { "CHANELLEN",   &SupportInfo::setUnsignedParam<&SupportInfo::channellen> },
        { "CHANTYPES",   &SupportInfo::setTokensParam<&SupportInfo::chantypes> },
        { "EXCEPTS",     &SupportInfo::setCharParam<&SupportInfo::excepts, 'e'> },
        { "IDCHAN",      &SupportInfo::setIdchanParam },
        { "INVEX",       &SupportInfo::setCharParam<&SupportInfo::invex, 'I'> },
        { "KICKLEN",     &SupportInfo::setUnsignedParam<&SupportInfo::kicklen> },
        { "MAXLIST",     &SupportInfo::setMaxlistParam },
        { "MODES",       &SupportInfo::setUnsignedParam<&SupportInfo::modes> },
        { "NETWORK",     &SupportInfo::setStringParam<&SupportInfo::network> },
        { "NICKLEN",     &SupportInfo::setUnsignedParam<&SupportInfo::nicklen> },
        { "PREFIX",      &SupportInfo::setPrefixParam },
        { "SAFELIST",    &SupportInfo::setBoolParam<&SupportInfo::safelist> },
        { "STATUSMSG",   &SupportInfo::setTokensParam<&SupportInfo::statusmsg> },
        { "STD",         &SupportInfo::setStringParam<&SupportInfo::std> },
        { "TARGMAX",     &SupportInfo::setTargmaxParam },
        { "TOPICLEN",    &SupportInfo::setUnsignedParam<&SupportInfo::topiclen> },
        { "MAXCHANNELS", &SupportInfo::setMaxchannelsParam },
        { "MAXBANS",     &SupportInfo::setMaxbansParam },
        { "WALLCHOPS",   &SupportInfo::addToken<&SupportInfo::statusmsg, '@'> },
        { "WALLVOICES",  &SupportInfo::addToken<&SupportInfo::statusmsg, '+'> },
        { "ELIST",       &SupportInfo::setTokensParam<&SupportInfo::elist> },
        { "CHIDLEN",     &SupportInfo::setChidlenParam },
        { "SILENCE",     &SupportInfo::setUnsignedParam<&SupportInfo::silence> },
        { "RFC2812",     &SupportInfo::setBoolParam<&SupportInfo::rfc2812> },
        { "PENALTY",     &SupportInfo::setBoolParam<&SupportInfo::penalty> },
        { "FNC",         &SupportInfo::setBoolParam<&SupportInfo::fnc> },
        { "AWAYLEN",     &SupportInfo::setUnsignedParam<&SupportInfo::awaylen> },
        //{"NOQUIT",      &SupportInfo::setBoolParam<&SupportInfo::noquit>},
        { "USERIP",      &SupportInfo::setBoolParam<&SupportInfo::userip> },
        { "CPRIVMSG",    &SupportInfo::setBoolParam<&SupportInfo::cprivmsg> },
        { "CNOTICE",     &SupportInfo::setBoolParam<&SupportInfo::cnotice> },
        { "MAXTARGETS",  &SupportInfo::setUnsignedParam<&SupportInfo::maxtargets> },
        { "KNOCK",       &SupportInfo::setBoolParam<&SupportInfo::knock> },
        { "VCHANS",      &SupportInfo::setBoolParam<&SupportInfo::vchans> },
        { "WATCH",       &SupportInfo::setUnsignedParam<&SupportInfo::watch> },
        { "WHOX",        &SupportInfo::setBoolParam<&SupportInfo::whox> },
        { "CALLERID",    &SupportInfo::setBoolParam<&SupportInfo::callerid> }
    };

    Handler handler = map[name];
    if (handler != NULL)
        (this->*handler)(value);

    tokens.insert(name, value);
}

void SupportInfo::clearParam(const QString& name)
{
    this->setDefaults();

    function<void(SupportInfo*)> setter = defaults.value(name);
    if (setter)
        setter(this);
}

void SupportInfo::setDefaults()
{
    if (!defaults.empty())
        return;

    // We could have used templates instead of bound functions like for the non-defaults, but this would
    // prevent us from using initializer-lists for collections, among other things.
    this->setDefault("CASEMAPPING", &SupportInfo::casemapping, Rfc1459);
    this->setDefault("CHANLIMIT", &SupportInfo::chanlimit);
    defaults.insert("CHANMODES", bind(&SupportInfo::clearChanmodes, _1));
    this->setDefault("CHANELLEN", &SupportInfo::channellen, 200U);
    this->setDefault<QSet<QChar> >("CHANTYPES", &SupportInfo::chantypes, { '#', '&' });
    this->setDefault("EXCEPTS", &SupportInfo::excepts);
    this->setDefault("IDCHAN", &SupportInfo::idchan);
    this->setDefault("INVEX", &SupportInfo::invex);
    this->setDefault("KICKLEN", &SupportInfo::kicklen);
    this->setDefault("MAXLIST", &SupportInfo::maxlist);
    this->setDefault("MODES", &SupportInfo::modes, 3U);
    this->setDefault("NETWORK", &SupportInfo::network);
    this->setDefault("NICKLEN", &SupportInfo::nicklen, 9U);
    this->setDefault<QList<pair<QChar, QChar> > >("PREFIX", &SupportInfo::prefix, { { 'o','@' },{ 'v','+' } });
    this->setDefault("SAFELIST", &SupportInfo::safelist);
    this->setDefault("STATUSMSG", &SupportInfo::statusmsg);
    this->setDefault("STD", &SupportInfo::std);
    this->setDefault("TARGMAX", &SupportInfo::targmax);
    this->setDefault("TOPICLEN", &SupportInfo::topiclen);

    defaults.insert("WALLCHOPS", bind(&SupportInfo::removeToken, _1, &SupportInfo::statusmsg, '@'));
    defaults.insert("WALLVOICES", bind(&SupportInfo::removeToken, _1, &SupportInfo::statusmsg, '+'));
    this->setDefault("ELIST", &SupportInfo::elist);
    defaults.insert("CHIDLEN", bind(&SupportInfo::clearChidlen, _1));
    this->setDefault("SILENCE", &SupportInfo::silence);
    this->setDefault("RFC2812", &SupportInfo::rfc2812);
    this->setDefault("PENALTY", &SupportInfo::penalty);
    this->setDefault("FNC", &SupportInfo::fnc);
    this->setDefault("AWAYLEN", &SupportInfo::awaylen);
    this->setDefault("USERIP", &SupportInfo::userip);
    this->setDefault("CPRIVMSG", &SupportInfo::cprivmsg);
    this->setDefault("CNOTICE", &SupportInfo::cnotice);
    this->setDefault("MAXTARGETS", &SupportInfo::maxtargets, 1U);
    this->setDefault("KNOCK", &SupportInfo::knock);
    this->setDefault("VCHANS", &SupportInfo::vchans);
    this->setDefault("WATCH", &SupportInfo::watch);
    this->setDefault("WHOX", &SupportInfo::whox);
    this->setDefault("CALLERID", &SupportInfo::callerid);
}

template <typename ParamType>
void SupportInfo::setDefault(const QString& name, ParamType SupportInfo::* param, const ParamType& dflt)
{
    defaults.insert(name, bind(&SupportInfo::doClearParam<ParamType>, _1, param, dflt));
}

template <typename ParamType>
void SupportInfo::doClearParam(ParamType SupportInfo::* param, const ParamType& dflt)
{
    this->*param = dflt;
}

void SupportInfo::clearChanmodes()
{
    this->chanmodesA.clear();
    this->chanmodesB.clear();
    this->chanmodesC.clear();
    this->chanmodesD.clear();
}

void SupportInfo::removeToken(QSet<QChar> SupportInfo::* var, QChar c)
{
    (this->*var).remove(c);
}

void SupportInfo::clearChidlen()
{
    this->idchan.remove('!');
}

template <bool SupportInfo::* var>
void SupportInfo::setBoolParam(const QString& unused)
{
    this->*var = true;
    Q_UNUSED(unused);
}

template <QChar SupportInfo::* var, char dflt>
void SupportInfo::setCharParam(const QString& value)
{
    if (value.isEmpty())
        this->*var = dflt;
    else
        this->*var = value.at(0);
}

template <uint SupportInfo::* var>
void SupportInfo::setUnsignedParam(const QString& value)
{
    this->*var = value.toUInt();
}

template <QString SupportInfo::* var>
void SupportInfo::setStringParam(const QString& value)
{
    this->*var = value;
}

void SupportInfo::setTokensParam(QSet<QChar> SupportInfo::* param, const QString& value)
{
    (this->*param).clear();
    for (QChar c : value)
        (this->*param).insert(c);
}

template <QSet<QChar> SupportInfo::* var>
void SupportInfo::setTokensParam(const QString& value)
{
    this->setTokensParam(var, value);
}

template <QSet<QChar> SupportInfo::* var, char c>
void SupportInfo::addToken(const QString& unused)
{
    (this->*var).insert(c);
    Q_UNUSED(unused);
}

void SupportInfo::setPrefixParam(const QString& value)
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

void SupportInfo::setChanmodesParam(const QString& value)
{
    QStringList parts = value.split(',');
    auto item = parts.cbegin(), end = parts.cend();
    QSet<QChar> SupportInfo::* sets[] = {
        &SupportInfo::chanmodesA, 
        &SupportInfo::chanmodesB, 
        &SupportInfo::chanmodesC, 
        &SupportInfo::chanmodesD 
    };

    for (auto var : sets)
    {
        if (item == end) return;    // fewer than four definitions
        this->setTokensParam(var, *item++);
    }
}

void SupportInfo::setChanlimitParam(const QString& value)
{
    this->chanlimit.clear();

    auto list = parsePairList(value);

    for (auto item : list)
        for (QChar symbol : item.first)
            chanlimit.insert(symbol, item.second);
}

void SupportInfo::setMaxchannelsParam(const QString& value)
{
    this->chanlimit.clear();

    bool ok;
    int limit = value.toInt(&ok);
    if (!ok)
        return;

    for (QChar prefix : this->chantypes)
        this->chanlimit.insert(prefix, limit);
}

void SupportInfo::setMaxlistParam(const QString& value)
{
    this->maxlist = parsePairList(value);
}

void SupportInfo::setMaxbansParam(const QString& value)
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

void SupportInfo::setIdchanParam(const QString& value)
{
    auto list = parsePairList(value);

    for (auto item : list)
        for (QChar symbol : item.first)
            this->idchan.insert(symbol, item.second);
}

void SupportInfo::setChidlenParam(const QString& value)
{
    this->idchan['!'] = value.toUInt();
}

void SupportInfo::setTargmaxParam(const QString& value)
{
    auto list = parsePairList(value);
    for (auto item : list)
        this->targmax.insert(item.first, item.second);
}

QList<pair<QString, int> > SupportInfo::parsePairList(const QString& value)
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

void SupportInfo::setCasemappingParam(const QString& value)
{
    if (value == "ascii")
        casemapping = Ascii;
    else if (value == "strict-rfc1459")
        casemapping = StrictRfc1459;
    else
        casemapping = Rfc1459;
}

QString SupportInfo::unescapeValue(const QStringRef& range)
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

QChar SupportInfo::unescapeChar(QString::const_iterator& it, const QString::const_iterator& end)
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
