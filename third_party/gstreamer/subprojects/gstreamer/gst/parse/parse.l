%{
#include "third_party/gstreamer/subprojects/gstreamer/gst/gst_private.h"

#include <math.h>
#include <string.h>

#include "third_party/glib/glib/gprintf.h"

#include "third_party/gstreamer/subprojects/gstreamer/gst/parse/types.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstinfo.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gsturi.h"
#include "grammar.tab.h"

#ifdef malloc
#undef malloc
#endif

#ifdef free
#undef free
#endif

#ifdef realloc
#undef realloc
#endif

#define malloc g_malloc
#define free g_free
#define realloc g_realloc

/* Override the default ECHO so as to avoid fortify warnings. Ignore the
   embedded-NUL case for now. We know yytext is NUL-terminated. */
#define ECHO g_fprintf(yyout, "%s", yytext)

#ifdef G_HAVE_ISO_VARARGS
#define PRINT(...) GST_CAT_DEBUG (GST_CAT_PIPELINE, "flex: " __VA_ARGS__)
#elif defined(G_HAVE_GNUC_VARARGS)
#define PRINT(args...) GST_CAT_DEBUG (GST_CAT_PIPELINE, "flex: " args)
#else
static inline void
PRINT (const char *format, ...)
{
  va_list varargs;

  va_start (varargs, format);
  GST_CAT_LEVEL_LOG_valist (GST_CAT_PIPELINE, GST_LEVEL_DEBUG, NULL,
    format, varargs);
  va_end (varargs);
}
#endif

%}

_operator [(){}.!:,;=]
_identifier [[:alnum:]_][[:alnum:]\-_%:]*

_char ("\\".)|([^[:space:]])
_string {_char}+|("\""([^\"]|"\\\"")*"\"")|("'"([^']|"\\\'")*"'")

_assign [[:space:]]*"="[[:space:]]*

_protocol [[:alpha:]][[:alnum:]+-\.]*
_url ({_protocol}"://"{_string}|["."{_identifier}]?"/"{_string})|({_protocol}"://")

/* we must do this here, because nearly everything matches a {_string} */ 
_assignment {_identifier}{_assign}{_string}

_preset @preset{_assign}{_string}

/* get pad/element references and stuff with dots right */
_padref "."{_identifier}
_ref {_identifier}"."{_identifier}?
_binref {_identifier}[[:space:]]*"."[[:space:]]*"("

/* links */
_mimechar [[:alnum:]-]
_mimetype {_mimechar}+"/"{_mimechar}+
_capschar ("\\".)|([^\;!])
_capsstring {_capschar}+
_caps {_mimetype}(","[^!]|{_capsstring})*
_link ([!:][[:space:]]*{_caps}([[:space:]]*(";"[[:space:]]*{_caps})*[[:space:]]*)*[!:])|([!:])

%x value
%option noyywrap
%option nounput
%option reentrant
%option bison-bridge
%option never-interactive
%option noinput
%%

{_assignment} {
    /* "=" */
    PRINT ("ASSIGNMENT: %s", yytext);
    yylval->ss = gst_parse_strdup (yytext);
    BEGIN (INITIAL);
    return ASSIGNMENT;
}

{_preset} {
    gchar *pos = yytext;
    while (*pos && (*pos != '=')) pos++;
    while (*pos && g_ascii_isspace (*pos)) pos++;
    PRINT ("PRESET: %s", &pos[1]);
    yylval->ss = gst_parse_strdup (&pos[1]);
    BEGIN (INITIAL);
    return PRESET;
}

{_padref} {
    yytext++;
    PRINT ("PADREF: %s", yytext);
    yylval->ss = gst_parse_strdup (yytext);
    BEGIN (INITIAL);
    return PADREF;
}

{_ref} {
    PRINT ("REF: %s", yytext);
    yylval->ss = gst_parse_strdup (yytext);
    BEGIN (INITIAL);
    return REF;
}

{_binref} {
    gchar *pos = yytext;
    while (!g_ascii_isspace (*pos) && (*pos != '.')) pos++;
    *pos = '\0';
    PRINT ("BINREF: %s", yytext);
    yylval->ss = gst_parse_strdup (yytext);
    BEGIN (INITIAL);
    return BINREF;
}

{_identifier} {
    PRINT ("IDENTIFIER: %s", yytext);
    yylval->ss = gst_parse_strdup (yytext);
    BEGIN (INITIAL);
    return IDENTIFIER;
}

{_link} {
    gchar *c = yytext;
    gchar op;
    gboolean link_all;

    PRINT ("LINK: %s", yytext);
    /* First char is a link operator */
    link_all = (*c == ':');
    c++;
    if (*c) {
      while (g_ascii_isspace (*c)) c++;
      c = yylval->ss = gst_parse_strdup (c);
      while (*c) c++;
      /* Last non-space char is a link operator */
      op = *--c;
      if (op != '!' && op != ':')
	g_assert_not_reached ();
      if (op == ':')
        link_all = TRUE;

      /* Walk backward past whitespaces - what remains
       * is a filter caps string, or empty */
      while (g_ascii_isspace (*--c));
      *++c = '\0';
    } else {
      yylval->ss = NULL;
    }
    BEGIN (INITIAL);
    return link_all ? LINK_ALL : LINK;
}
{_url} {
  PRINT ("URL: %s", yytext);
  yylval->ss = g_strdup (yytext);
  gst_parse_unescape (yylval->ss);
  BEGIN (INITIAL);
  return PARSE_URL;
}

{_operator} { PRINT ("OPERATOR: [%s]", yytext); return *yytext; }

[[:space:]]+ { PRINT ("SPACE: [%s]", yytext); }

. {
    PRINT ("Invalid Lexer element: %s\n", yytext);
    return *yytext;
}

%%
