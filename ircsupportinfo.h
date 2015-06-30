#ifndef IRCSUPPORTINFO_H
#define IRCSUPPORTINFO_H

#include <QChar>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QSet>

struct IrcSupportInfo
{
    enum CaseMapping
    {
        Ascii, Rfc1459, StrictRfc1459
    };

    IrcSupportInfo();
    ~IrcSupportInfo();

    void parseISupport(QStringList params);

    // ISUPPORT
    QMap<QChar, QChar> prefix;
    QSet<QChar> chantypes;
    QSet<QChar> chanmodesA;    // nick/address modes such as 'b', always take a parameter
    QSet<QChar> chanmodesB;    // chan setting modes with parameters (e.g. 'k')
    QSet<QChar> chanmodesC;    // chan settings, takes a parameter when set
    QSet<QChar> chanmodesD;    // no parameters
    uint modes;
    uint maxchannels;
    QMap<QChar, uint> chanlimit;
    uint nicklen;
    QMap<QChar, uint> maxlist;
    QString network;
    QChar excepts;
    QChar invex;
    QString statusmsg;
    CaseMapping casemapping;
    QString elist;
    uint topiclen;
    uint kicklen;
    uint channellen;
    QMap<QChar, uint> idchan;
    QString std;
    uint silence;
    bool rfc2812;
    bool penalty;
    bool fnc;
    bool safelist;
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

private:
    QString unescapeValue(QString val);
};

#endif // IRCSUPPORTINFO_H
