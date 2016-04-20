#include "message.h"

#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QMap>

namespace Irc
{

//IrcMessage::IrcMessage()
//{
//    this->_command.replyCode = NON_NUMERIC;
//}

Message::Message(const QByteArray& raw)
{
    if (!this->parse(QString(raw)))
    {   // make invalid
        this->_command.string = QString();
        this->_command.replyCode = NON_NUMERIC;
    }
}

Message::Message(const QString& command, const QStringList& params, const QMap<QString, QString>& tags, const QString& prefix)
    : _tags(tags), _prefix(prefix), _params(params)
{
    this->_command.string = command;
    this->_command.replyCode = NON_NUMERIC;
}

Message::Message(ReplyCode replyCode, const QStringList& params, const QMap<QString, QString>& tags, const QString& prefix)
    : _tags(tags), _prefix(prefix), _params(params)
{
    this->_command.replyCode = replyCode;
}

Message::Message(const QString& command, const std::initializer_list<QString> params, const QMap<QString, QString>& tags, const QString& prefix)
    : _tags(tags), _prefix(prefix)
{
    this->_command.string = command;
    this->_command.replyCode = NON_NUMERIC;

    for (QString param : params)
        this->_params.append(param);
}

Message::Message(ReplyCode replyCode, const std::initializer_list<QString> params, const QMap<QString, QString>& tags, const QString& prefix)
    : _tags(tags), _prefix(prefix)
{
    this->_command.replyCode = replyCode;

    for (QString param : params)
        this->_params.append(param);
}

Message::~Message()
{
}

bool Message::isValid() const
{
    return !(this->_command.string.isEmpty()
             && this->_command.replyCode == NON_NUMERIC);
}

const QMap<QString, QString>& Message::tags() const
{
    return this->_tags;
}

const QString& Message::prefix() const
{
    return this->_prefix;
}

bool Message::isNumeric() const
{
    return this->_command.string.isEmpty();
}

const QString& Message::command() const
{
    return this->_command.string;
}

Message::ReplyCode Message::replyCode() const
{
    return this->_command.replyCode;
}

const QStringList& Message::params() const
{
    return this->_params;
}

bool Message::parse(const QString &raw)
{
    auto it = raw.constBegin(), end = raw.constEnd();

    // [ircv3.2] <message> ::= ['@' <tags> <SPACE>] [':' <prefix> <SPACE> ] <command> <params> <crlf>
    if (it == end)
        return false;   // nothing to parse

    if (*it == '@')
    {
        if (!this->parseTags(++it, end))
            return false;   // no valid tags after '@'
        if (it == end || *it++ != ' ')
            return false;   // missing space after tags
    }

    if (*it == ':')
    {
        if (!this->parsePrefix(++it, end))
            return false;   // no valid prefix after ':'

        // dereferencing NULL pointer 'it': I don't think that QString::constBegin ever returns NULL.
#       pragma warning(suppress:6011)
        if (it == end || *it++ != ' ')
            return false;   // missing space after prefix
    }

    if (!this->parseCommand(it, end))
        return false;   // failed to parse command

    if (it != end && !this->parseParams(it, end))
        return false;   // failed to parse params

    if (*it++ != '\r' || *it != '\n')
        return false;   // unexpected character

    return true;
}

bool Message::parseTags(QString::const_iterator& it, const QString::const_iterator& end)
{   // [ircv3.2] <tags> ::= <tag> [';' <tag>]*
    if (it == end)
        return false;   // nothing after '@'

    if (!this->parseTag(it, end))   // first tag
        return false;   // invalid tag after '@'

    while (*it == ';')
        // more tags
        if (!this->parseTag(++it, end))
            return false;   // invalid tag after ';'

    return true;
}

bool Message::parseTag(QString::const_iterator& it, const QString::const_iterator& end)
{   // [ircv3.2] <tag> ::= <key> ['=' <escaped value>]

    QString key, value;

    if (it == end)
        return false;   // expected a tag, not end

    if (!this->parseTagKey(it, end, key))
        return false;

    if (it == end || (*it == '=' && !this->parseTagValue(++it, end, value)))
        return false;

    this->_tags.insert(key, value);
    return true;
}

bool Message::parseTagKey(QString::const_iterator& it, const QString::const_iterator& end, QString& key)
{   // [ircv3.2]
    // <key>    ::= [ <vendor> '/' ] <sequence of letters, digits, hyphens (`-`)>
    // <vendor> ::= <host>
    // deviation: in this case, we parse <host> as a sequence of letters,
    //   digits, hyphens, periods and colons. we don't check if it's well-formed.

    bool hasVendor = false;
    bool mustHaveVendor = false;

    QString::const_iterator start;
    for (start = it; it != end; ++it)
    {
        QChar c = *it;

        if (c != '-' && !c.isLetterOrNumber())
        {
            if (c == '.' || c == ':')
            {
                if (!hasVendor)
                    mustHaveVendor = true;
                else
                    break;
            }
            else if (c == '/')
            {
                if (!hasVendor)
                    hasVendor = true;
                else
                    return false;   // slash may only appear once in tag key
            }
            else
                break;
        }

        //key += c;
    }

    if (mustHaveVendor && !hasVendor)
        return false;   // invalid period or colon in tag key

    key = QString(start, it - start);

    return true;
}

bool Message::parseTagValue(QString::const_iterator& it, const QString::const_iterator& end, QString& value)
{   // [ircv3.2] <escaped value> ::= <sequence of any characters except NUL, CR, LF, semicolon (`;`) and SPACE>
    for (; it != end; ++it)
    {
        QChar c = *it;

        if (c == '\0' || c == '\r' || c == '\n' || c == ';' || c == ' ')
            break;

        if (c == '\\')
        {   // <http://ircv3.net/specs/core/message-tags-3.2.html> under "Escaping Values"
            QChar escape = *it++;
            switch (escape.unicode())
            {
            case ':': c = ';'; break;
            case 's': c = ' '; break;
            case '\\': break;
            case 'r': c = '\r'; break;
            case 'n': c = '\n'; break;
            default: return false;  // invalid escape sequence
            }
        }

        value += c;
    }

    return true;
}

bool Message::parsePrefix(QString::const_iterator& it, const QString::const_iterator& end)
{   // [rfc2812] prefix =  servername / ( nickname [ [ "!" user ] "@" host ] )
    // deviation: we read everything until space. the address will be parsed later if needed.

    if (it == end || *it == ' ')
        return false;   // no prefix after ':'

    //for (; it != end && *it != ' '; ++it)
    //    this->_prefix += *it;

    QString::const_iterator start = it;
    while (it != end && *it != ' ')
        ++ it;

    this->_prefix = QString(start, it - start);

    return true;
}

bool Message::parseCommand(QString::const_iterator& it, const QString::const_iterator& end)
{   // [rfc2812] command =  1*letter / 3digit
    if (it == end)
        return false;

    if (it->isDigit())
    {
        if (it + 3 > end)
            return false;   // not enough characters for number

        // I'd rather not copy, but Qt can't parse out of an iterator range, and boost::spirit::qi struggles with QChar
        bool ok;
        this->_command.replyCode = static_cast<ReplyCode>(QString(it, 3).toUShort(&ok));

        it += 3;
        return ok;
    }
    else
    {
        //for (; it != end && it->isLetter(); ++it)
        //    this->_command.string += *it;

        QString::const_iterator start = it;
        while (it != end && it->isLetter())
            ++it;

        this->_command.string = QString(start, it - start);
    }

    return true;
}

bool Message::parseParams(QString::const_iterator& it, const QString::const_iterator& end)
{   // [rfc2812]
    // params =  *14( SPACE middle ) [ SPACE ":" trailing ]
    //        =/ 14( SPACE middle ) [ SPACE [ ":" ] trailing ]

    uchar paramCount;
    for (paramCount = 1; it != end && paramCount < 15; ++paramCount)
    {
        if (*it != ' ')
            break;  // no space, no parameter.
        if (++it == end)
            return false;
        if (*it == ':')
        {   // ':' introduces a "trailing" parameter that allows spaces
            if (!this->parseParam(++it, end, true))
                return false;   // expected a valid parameter after ':'

            return true;    // no parameter may follow a "trailing"
        }

        if (!this->parseParam(it, end, false))
            return false;   // expected a valid parameter after space
    }

    if (it != end && *it == ' ')
    {   // 15th parameter, rest of the message gets thrown in regardless of spaces
        if (*++it == ':') ++it;   // still consume ':' if there's one
        this->parseParam(it, end, true);
    }

    return true;
}

bool Message::parseParam(QString::const_iterator& it, const QString::const_iterator& end, bool trailing)
{   // [rfc2812]
    // middle     =  nospcrlfcl *( ":" / nospcrlfcl )
    // trailing   =  *( ":" / " " / nospcrlfcl )
    // nospcrlfcl =  %x01-09 / %x0B-0C / %x0E-1F / %x21-39 / %x3B-FF ; any octet except NUL, CR, LF, " " and ":"

    if (!trailing && (it == end || *it == ':' || *it == '\0' || *it == '\r' || *it == '\n' || *it == ' '))
        // middle parameters cannot be empty but trailing ones can. this might be a quirk in the RFC
        return false;

    QString::const_iterator start = it;
    while (it != end && (trailing || *it != ' ') && *it != '\r' && *it != '\n' && *it != '\0')
        ++it;

    //this->_params.append(param);
    this->_params.append(QString(start, it - start));

    return true;
}

QString Message::render() const
{
    QString out;
    if (!this->_tags.empty())
    {
        out.append('@');

        typedef QMap<QString, QString>::const_iterator it;
        for (it i = this->_tags.constBegin(); i != this->_tags.constEnd(); i++)
        {
            out.append(i.key());
            out.append(';');
            out.append(escapeTagValue(i.value()));
        }

        out.append(' ');
    }

    if (!this->_prefix.isEmpty())
    {
        out.append(':');
        out.append(this->_prefix);
        out.append(' ');
    }

    if (this->isNumeric())
    {
        QString str = QString("%1").arg(static_cast<int>(this->_command.replyCode), 3, 10, QChar('0'));
        out.append(str);
    }
    else
        out.append(this->_command.string);

    QStringListIterator i (this->_params);
    while (i.hasNext())
    {
        out.append(' ');
        QString param = i.next();
        if (param.contains(' '))
        {
            if (!i.hasNext())
                // TODO: warn.
                qt_noop();
            out.append(':');
        }

        out.append(param);
    }

    out.append("\r\n");

    return out;
}

QString Message::escapeTagValue(const QString& value)
{
    QString escaped;
    for (QChar c : value)
    {   // <http://ircv3.net/specs/core/message-tags-3.2.html>
        if (c == ';') escaped.append("\\:");
        else if (c == ' ') escaped.append("\\s");
        else if (c == '\\') escaped.append("\\\\");
        else if (c == '\r') escaped.append("\\r");
        else if (c == '\n') escaped.append("\\n");
        else escaped.append(c);
    }

    return escaped;
}

}
