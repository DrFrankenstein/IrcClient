#ifndef IRC_SUPPORTINFO_H
#define IRC_SUPPORTINFO_H

#include <functional>
#include <utility>
#include <algorithm>

#include <QChar>
#include <QString>
#include <QStringRef>
#include <QList>
#include <QStringList>
#include <QMap>
#include <QSet>

namespace Irc
{

class SupportInfo
{
public:
    enum CaseMapping
    {
        Ascii, Rfc1459, StrictRfc1459
    };

    SupportInfo();
    ~SupportInfo();

    template <typename InputIterator>
    void parseISupport(const InputIterator& begin, const InputIterator& end)
    {
        std::for_each(begin, end, std::bind(&SupportInfo::parseToken,
                                            this, std::placeholders::_1));
    }

private:
    // ISUPPORT
    CaseMapping casemapping;
    QMap<QChar, int> chanlimit;
    QSet<QChar> chanmodesA;    // nick/address modes such as 'b', always take a parameter
    QSet<QChar> chanmodesB;    // chan setting modes with parameters (e.g. 'k')
    QSet<QChar> chanmodesC;    // chan settings, takes a parameter when set
    QSet<QChar> chanmodesD;    // no parameters
    uint channellen;QString val;
    QSet<QChar> chantypes;
    QChar excepts;
    QMap<QChar, uint> idchan;
    QChar invex;
    uint kicklen;
    QList<std::pair<QString, int> > maxlist;
    uint modes;
    QString network;
    uint nicklen;
    QList<std::pair<QChar, QChar> > prefix;
    bool safelist;
    QSet<QChar> statusmsg;
    QString std;
    QHash<QString, int> targmax;
    uint topiclen;

    QSet<QChar> elist;
    uint silence;
    bool rfc2812;
    bool penalty;
    bool fnc;
    uint awaylen;
    bool userip;
    bool cprivmsg;
    bool cnotice;
    uint maxtargets;
    bool knock;
    bool vchans;
    uint watch;
    bool whox;
    bool callerid;

    // ircv3.1
    bool multi_prefix;
    bool sasl;
    bool account_notify;
    bool away_notify;
    bool extended_join;
    bool tls;

    // ircv3.2
    bool ircv32;
    bool metadata;
    bool monitor;
    bool account_tag;
    bool batch;
    bool cap_notify;
    bool chghost;
    bool echo_message;
    bool invite_notify;
    QStringList saslmechs;
    bool server_time;
    bool userhost_in_names;

    // ircv3.3 EXPERIMENTAL
    bool intents;

    QHash<QString, QString> tokens;

    static QHash<QString, std::function<void(SupportInfo*)> > defaults;

    void parseToken(const QString& token);
    void setParam(const QString& name, const QString& value);

    // clearing and defaults
    void clearParam(const QString& name);
    void setDefaults();
    template <typename ParamType>
        void setDefault(const QString& name, ParamType SupportInfo::* param, const ParamType& dflt = ParamType());
    template <typename ParamType>
        void doClearParam(ParamType SupportInfo::* param, const ParamType& dflt = ParamType());
    template <typename ParamType, ParamType SupportInfo::* var, const ParamType& dflt>
        void doClearParam();
    void clearChanmodes();
    void removeToken(QSet<QChar> SupportInfo::*var, QChar c);
    void clearChidlen();

    // generic parsers/setters
    template <bool SupportInfo::* var>
        void setBoolParam(const QString& unused);
    template <QChar SupportInfo::* var, char dflt = '\0'>
        void setCharParam(const QString& value);
    template <uint SupportInfo::* var>
        void setUnsignedParam(const QString& value);
    template <QString SupportInfo::* var>
        void setStringParam(const QString& value);
    void setTokensParam(QSet<QChar> SupportInfo::* param, const QString& value);
    template <QSet<QChar> SupportInfo::* var>
        void setTokensParam(const QString& value);
    template <QSet<QChar> SupportInfo::* var, char c>
        void addToken(const QString& unused);

    // parameter-specific parsers/setters
    void setPrefixParam(const QString& value);
    void setChanmodesParam(const QString& value);
    void setChanlimitParam(const QString& value);
    void setMaxchannelsParam(const QString& value);
    void setMaxlistParam(const QString& value);
    void setMaxbansParam(const QString& value);
    void setIdchanParam(const QString& value);
    void setChidlenParam(const QString& value);
    void setTargmaxParam(const QString& value);
    void setCasemappingParam(const QString& value);

    static QList<std::pair<QString, int> > parsePairList(const QString& value);
    static QString unescapeValue(const QStringRef& range);
    static QChar unescapeChar(QString::const_iterator& it, const QString::const_iterator& end);
};

}

#endif // IRC_SUPPORTINFO_H
