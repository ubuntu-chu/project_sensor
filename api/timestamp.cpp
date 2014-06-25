#include "timestamp.h"
#include "../includes/service.h"
#include <ctype.h>

Timestamp::Timestamp(time_t microSecondsSinceEpoch)
  : microSecondsSinceEpoch_(microSecondsSinceEpoch)
{
}

string Timestamp::toString() const
{
  char buf[32] = {0};
  time_t seconds = microSecondsSinceEpoch_ / kAccuracyPerSecond;
  time_t microseconds = microSecondsSinceEpoch_ % kAccuracyPerSecond;
  snprintf(buf, sizeof(buf)-1, "%d.%06d ", seconds, microseconds);
  return buf;
}

string Timestamp::toFormattedString() const
{
  char buf[32] = {0};
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kAccuracyPerSecond);
  int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kAccuracyPerSecond);
  struct tm tm_time;
  gmtime_r(&seconds, &tm_time);

  snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
      tm_time.tm_year + TM_YEAR_BASE, tm_time.tm_mon + 1, tm_time.tm_mday,
      tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
      microseconds);
  return buf;
}

Timestamp Timestamp::now()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  time_t seconds = tv.tv_sec;
  
  return Timestamp(seconds * kAccuracyPerSecond + tv.tv_usec);
}

Timestamp Timestamp::invalid()
{
  return Timestamp();
}

#ifdef def_MINI_TIME_LIBRARY

/* days per month -- nonleap! */
const short __spm[13] =
  { 0,
    (31),
    (31+28),
    (31+28+31),
    (31+28+31+30),
    (31+28+31+30+31),
    (31+28+31+30+31+30),
    (31+28+31+30+31+30+31),
    (31+28+31+30+31+30+31+31),
    (31+28+31+30+31+30+31+31+30),
    (31+28+31+30+31+30+31+31+30+31),
    (31+28+31+30+31+30+31+31+30+31+30),
    (31+28+31+30+31+30+31+31+30+31+30+31),
  };
static long int timezone;
static const char days[] = "Sun Mon Tue Wed Thu Fri Sat ";
static const char months[] = "Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec ";

/* seconds per day */
#define SPD 24*60*60

int __isleap(int year)
{
	/* every fourth year is a leap year except for century years that are
	 * not divisible by 400. */
	/*  return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)); */
	return (!(year % 4) && ((year % 100) || !(year % 400)));
}

struct tm *gmtime_r(const time_t *timep, struct tm *r)
{
	time_t i;
	register time_t work = *timep % (SPD);
	r->tm_sec = work % 60;
	work /= 60;
	r->tm_min = work % 60;
	r->tm_hour = work / 60;
	work = *timep / (SPD);
	r->tm_wday = (4 + work) % 7;
	for (i = 1970;; ++i)
	{
		register time_t k = __isleap(i) ? 366 : 365;
		if (work >= k)
			work -= k;
		else
			break;
	}
	r->tm_year = i - TM_YEAR_BASE;
	r->tm_yday = work;

	r->tm_mday = 1;
	if (__isleap(i) && (work > 58))
	{
		if (work == 59)
			r->tm_mday = 2; /* 29.2. */
		work -= 1;
	}

	for (i = 11; i && (__spm[i] > work); --i)
		;
	r->tm_mon = i;
	r->tm_mday += work - __spm[i];
	return r;
}

struct tm* localtime_r(const time_t* t, struct tm* r)
{
	time_t tmp;
	struct timezone tz = {0};
	gettimeofday(0, &tz);
	timezone = tz.tz_minuteswest * 60L;
	tmp = *t + timezone;
	return gmtime_r(&tmp, r);
}

struct tm* localtime(const time_t* t)
{
	static struct tm tmp;
	return localtime_r(t, &tmp);
}

time_t mktime(struct tm * const t)
{
	register time_t day;
	register time_t i;
	register time_t years = t->tm_year - 70;

	if (t->tm_sec > 60)
	{
		t->tm_min += t->tm_sec / 60;
		t->tm_sec %= 60;
	}
	if (t->tm_min > 60)
	{
		t->tm_hour += t->tm_min / 60;
		t->tm_min %= 60;
	}
	if (t->tm_hour > 24)
	{
		t->tm_mday += t->tm_hour / 24;
		t->tm_hour %= 24;
	}
	if (t->tm_mon > 12)
	{
		t->tm_year += t->tm_mon / 12;
		t->tm_mon %= 12;
	}
	while (t->tm_mday > __spm[1 + t->tm_mon])
	{
		if (t->tm_mon == 1 && __isleap(t->tm_year + TM_YEAR_BASE))
		{
			--t->tm_mday;
		}
		t->tm_mday -= __spm[t->tm_mon];
		++t->tm_mon;
		if (t->tm_mon > 11)
		{
			t->tm_mon = 0;
			++t->tm_year;
		}
	}

	if (t->tm_year < 70)
		return (time_t) -1;

	/* Days since 1970 is 365 * number of years + number of leap years since 1970 */
	day = years * 365 + (years + 1) / 4;

	/* After 2100 we have to substract 3 leap years for every 400 years
	 This is not intuitive. Most mktime implementations do not support
	 dates after 2059, anyway, so we might leave this out for it's
	 bloat. */
	if (years >= 131)
	{
		years -= 131;
		years /= 100;
		day -= (years >> 2) * 3 + 1;
		if ((years &= 3) == 3)
			years--;
		day -= years;
	}

	day += t->tm_yday = __spm[t->tm_mon] + t->tm_mday - 1 +
			(__isleap(t->tm_year + TM_YEAR_BASE) & (t->tm_mon > 1));

	/* day is now the number of days since 'Jan 1 1970' */
	i = 7;
	t->tm_wday = (day + 4) % i; /* Sunday=0, Monday=1, ..., Saturday=6 */

	i = 24;
	day *= i;
	i = 60;
	return ((day + t->tm_hour) * i + t->tm_min) * i + t->tm_sec;
}

static void num2str(char *c, int i)
{
	c[0] = i / 10 + '0';
	c[1] = i % 10 + '0';
}

char *asctime_r(const struct tm *t, char *buf)
{
	/* "Wed Jun 30 21:49:08 1993\n" */
	*(int*) buf = *(int*) (days + (t->tm_wday << 2));
	*(int*) (buf + 4) = *(int*) (months + (t->tm_mon << 2));
	num2str(buf + 8, t->tm_mday);
	if (buf[8] == '0')
		buf[8] = ' ';
	buf[10] = ' ';
	num2str(buf + 11, t->tm_hour);
	buf[13] = ':';
	num2str(buf + 14, t->tm_min);
	buf[16] = ':';
	num2str(buf + 17, t->tm_sec);
	buf[19] = ' ';
	num2str(buf + 20, (t->tm_year + TM_YEAR_BASE) / 100);
	num2str(buf + 22, (t->tm_year + TM_YEAR_BASE) % 100);
	buf[24] = '\n';
	return buf;
}

char *asctime(const struct tm *timeptr)
{
	static char buf[25];
	return asctime_r(timeptr, buf);
}

char *ctime(const time_t *timep)
{
	return asctime(localtime(timep));
}

/**	$OpenBSD: strptime.c,v 1.12 2008/06/26 05:42:05 ray Exp $ */
/**	$NetBSD: strptime.c,v 1.12 1998/01/20 21:39:40 mycroft Exp $	*/

/**-
 * Copyright (c) 1997, 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code was contributed to The NetBSD Foundation by Klaus Klein.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** OPENBSD ORIGINAL: lib/libc/time/strptime.c */

#ifndef HAVE_STRPTIME

/*
buf：                       tm格式化后的C字符串（以0结尾）；
format：                 字符串格式，构建方式与strftime的format字符串完全一样；
struct tm *timeptr：转换后存储tm值的指针；
返回值：                指向转换过程处理的最后一个字符后面的那个字符。

功能：
将一个字符串格式化为一个tm结构，功能与strptime相反。

format参数：

转换控制符 说明
%a 星期几的简写形式
%A 星期几的全称
%b 月份的简写形式
%B 月份的全称
%c 日期和时间
%d 月份中的日期,0-31
%H 小时,00-23
%I 12进制小时钟点,01-12
%j 年份中的日期,001-366
%m 年份中的月份,01-12
%M 分,00-59
%p 上午或下午
%s 秒,00-60
%u 星期几,1-7
%w 星期几,0-6
%x 当地格式的日期
%X 当地格式的时间
%y 年份中的最后两位数,00-99
%Y 年
%Z 地理时区名称
*/
#define	_ctloc(x)		(x)
/** #define	_ctloc(x)		(_CurrentTimeLocale->x) */

/**
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#define _ALT_E			0x01
#define _ALT_O			0x02
#define	_LEGAL_ALT(x)		{ if (alt_format & ~(x)) return (0); }

static const char *day[] = {
    "Sunday", "Monday", "Tuesday", "Wendesday", "Thursday", "Friday", "Saturday"
};
static const char *abday[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
static const char *mon[] = {
    "January", "February", "March", "April", "May", "June", "July", 
    "Aguest", "September", "October", "November", "December"
};
static const char *abmon[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};


static	int _conv_num(unsigned char **, int *, int, int);
static	char *_strptime(const char *, const char *, struct tm *, int);


char *
strptime(const char *buf, const char *fmt, struct tm *tm)
{
	return(_strptime(buf, fmt, tm, 1));
}

static char *
_strptime(const char *buf, const char *fmt, struct tm *tm, int initialize)
{
	unsigned char c;
	//const unsigned char *bp;
    unsigned char *bp;
	size_t len;
	int alt_format, i;
	static int century, relyear;

	if (initialize) {
		century = TM_YEAR_BASE;
		relyear = -1;
	}

	bp = (unsigned char *)buf;
	while ((c = *fmt) != '\0') {
		/** Clear `alternate' modifier prior to new conversion. */
		alt_format = 0;

		/** Eat up white-space. */
		if (isspace(c)) {
			while (isspace(*bp))
				bp++;

			fmt++;
			continue;
		}
				
		if ((c = *fmt++) != '%')
			goto literal;


again:		switch (c = *fmt++) {
		case '%':	/** "%%" is converted to "%". */
literal:
		if (c != *bp++)
			return (NULL);

		break;

		/**
		 * "Alternative" modifiers. Just set the appropriate flag
		 * and start over again.
		 */
		case 'E':	/** "%E?" alternative conversion modifier. */
			_LEGAL_ALT(0);
			alt_format |= _ALT_E;
			goto again;

		case 'O':	/** "%O?" alternative conversion modifier. */
			_LEGAL_ALT(0);
			alt_format |= _ALT_O;
			goto again;
			
		/**
		 * "Complex" conversion rules, implemented through recursion.
		 */
#if 0
		case 'c':	/** Date and time, using the locale's format. */
			_LEGAL_ALT(_ALT_E);
			if (!(bp = _strptime(bp, _ctloc(d_t_fmt), tm, 0)))
				return (NULL);
			break;
#endif
		case 'D':	/** The date as "%m/%d/%y". */
			_LEGAL_ALT(0);
			if (!(bp = (unsigned char *)_strptime((const char *)bp, "%m/%d/%y", tm, 0)))
				return (NULL);
			break;
	
		case 'R':	/** The time as "%H:%M". */
			_LEGAL_ALT(0);
			if (!(bp = (unsigned char *)_strptime((const char *)bp, "%H:%M", tm, 0)))
				return (NULL);
			break;

		case 'r':	/** The time as "%I:%M:%S %p". */
			_LEGAL_ALT(0);
			if (!(bp = (unsigned char *)_strptime((const char *)bp, "%I:%M:%S %p", tm, 0)))
				return (NULL);
			break;

		case 'T':	/** The time as "%H:%M:%S". */
			_LEGAL_ALT(0);
			if (!(bp = (unsigned char *)_strptime((const char *)bp, "%H:%M:%S", tm, 0)))
				return (NULL);
			break;
#if 0
		case 'X':	/** The time, using the locale's format. */
			_LEGAL_ALT(_ALT_E);
			if (!(bp = _strptime(bp, _ctloc(t_fmt), tm, 0)))
				return (NULL);
			break;

		case 'x':	/** The date, using the locale's format. */
			_LEGAL_ALT(_ALT_E);
			if (!(bp = _strptime(bp, _ctloc(d_fmt), tm, 0)))
				return (NULL);
			break;
#endif
		/**
		 * "Elementary" conversion rules.
		 */
#if 1
		case 'A':	/** The day of week, using the locale's form. */
		case 'a':
			_LEGAL_ALT(0);
			for (i = 0; i < 7; i++) {
				/** Full name. */
				len = strlen(_ctloc(day[i]));
				if (strncasecmp(_ctloc(day[i]), (const char *)bp, len) == 0)
					break;

				/** Abbreviated name. */
				len = strlen(_ctloc(abday[i]));
				if (strncasecmp(_ctloc(abday[i]), (const char *)bp, len) == 0)
					break;
			}

			/** Nothing matched. */
			if (i == 7)
				return (NULL);

			tm->tm_wday = i;
			bp += len;
			break;

		case 'B':	/** The month, using the locale's form. */
		case 'b':
		case 'h':
			_LEGAL_ALT(0);
			for (i = 0; i < 12; i++) {
				/** Full name. */
				len = strlen(_ctloc(mon[i]));
				if (strncasecmp(_ctloc(mon[i]), (const char *)bp, len) == 0)
					break;

				/** Abbreviated name. */
				len = strlen(_ctloc(abmon[i]));
				if (strncasecmp(_ctloc(abmon[i]), (const char *)bp, len) == 0)
					break;
			}

			/** Nothing matched. */
			if (i == 12)
				return (NULL);

			tm->tm_mon = i;
			bp += len;
			break;
#endif

		case 'C':	/** The century number. */
			_LEGAL_ALT(_ALT_E);
			if (!(_conv_num(&bp, &i, 0, 99)))
				return (NULL);

			century = i * 100;
			break;

		case 'd':	/** The day of month. */
		case 'e':
			_LEGAL_ALT(_ALT_O);
			if (!(_conv_num(&bp, &tm->tm_mday, 1, 31)))
				return (NULL);
			break;

		case 'k':	/** The hour (24-hour clock representation). */
			_LEGAL_ALT(0);
			/** FALLTHROUGH */
		case 'H':
			_LEGAL_ALT(_ALT_O);
			if (!(_conv_num(&bp, &tm->tm_hour, 0, 23)))
				return (NULL);
			break;

		case 'l':	/** The hour (12-hour clock representation). */
			_LEGAL_ALT(0);
			/** FALLTHROUGH */
		case 'I':
			_LEGAL_ALT(_ALT_O);
			if (!(_conv_num(&bp, &tm->tm_hour, 1, 12)))
				return (NULL);
			break;

		case 'j':	/** The day of year. */
			_LEGAL_ALT(0);
			if (!(_conv_num(&bp, &tm->tm_yday, 1, 366)))
				return (NULL);
			tm->tm_yday--;
			break;

		case 'M':	/** The minute. */
			_LEGAL_ALT(_ALT_O);
			if (!(_conv_num(&bp, &tm->tm_min, 0, 59)))
				return (NULL);
			break;

		case 'm':	/** The month. */
			_LEGAL_ALT(_ALT_O);
			if (!(_conv_num(&bp, &tm->tm_mon, 1, 12)))
				return (NULL);
			tm->tm_mon--;
			break;

#if 0
		case 'p':	/** The locale's equivalent of AM/PM. */
			_LEGAL_ALT(0);
			/** AM? */
			len = strlen(_ctloc(am_pm[0]));
			if (strncasecmp(_ctloc(am_pm[0]), bp, len) == 0) {
				if (tm->tm_hour > 12)	/** i.e., 13:00 AM ?! */
					return (NULL);
				else if (tm->tm_hour == 12)
					tm->tm_hour = 0;

				bp += len;
				break;
			}
			/** PM? */
			len = strlen(_ctloc(am_pm[1]));
			if (strncasecmp(_ctloc(am_pm[1]), bp, len) == 0) {
				if (tm->tm_hour > 12)	/** i.e., 13:00 PM ?! */
					return (NULL);
				else if (tm->tm_hour < 12)
					tm->tm_hour += 12;

				bp += len;
				break;
			}

			/** Nothing matched. */
			return (NULL);
#endif
		case 'S':	/** The seconds. */
			_LEGAL_ALT(_ALT_O);
			if (!(_conv_num(&bp, &tm->tm_sec, 0, 61)))
				return (NULL);
			break;

		case 'U':	/** The week of year, beginning on sunday. */
		case 'W':	/** The week of year, beginning on monday. */
			_LEGAL_ALT(_ALT_O);
			/**
			 * XXX This is bogus, as we can not assume any valid
			 * information present in the tm structure at this
			 * point to calculate a real value, so just check the
			 * range for now.
			 */
			 if (!(_conv_num(&bp, &i, 0, 53)))
				return (NULL);
			 break;

		case 'w':	/** The day of week, beginning on sunday. */
			_LEGAL_ALT(_ALT_O);
			if (!(_conv_num(&bp, &tm->tm_wday, 0, 6)))
				return (NULL);
			break;

		case 'Y':	/** The year. */
			_LEGAL_ALT(_ALT_E);
			if (!(_conv_num(&bp, &i, 0, 9999)))
				return (NULL);

			relyear = -1;
			tm->tm_year = i - TM_YEAR_BASE;
			break;

		case 'y':	/** The year within the century (2 digits). */
			_LEGAL_ALT(_ALT_E | _ALT_O);
			if (!(_conv_num(&bp, &relyear, 0, 99)))
				return (NULL);
			break;

		/**
		 * Miscellaneous conversions.
		 */
		case 'n':	/** Any kind of white-space. */
		case 't':
			_LEGAL_ALT(0);
			while (isspace(*bp))
				bp++;
			break;


		default:	/** Unknown/unsupported conversion. */
			return (NULL);
		}


	}

	/**
	 * We need to evaluate the two digit year spec (%y)
	 * last as we can get a century spec (%C) at any time.
	 */
	if (relyear != -1) {
		if (century == TM_YEAR_BASE) {
			if (relyear <= 68)
				tm->tm_year = relyear + 2000 - TM_YEAR_BASE;
			else
				tm->tm_year = relyear + TM_YEAR_BASE - TM_YEAR_BASE;
		} else {
			tm->tm_year = relyear + century - TM_YEAR_BASE;
		}
	}

	return ((char *)bp);
}


static int
_conv_num(unsigned char **buf, int *dest, int llim, int ulim)
{
	int result = 0;
	int rulim = ulim;

	if (**buf < '0' || **buf > '9')
		return (0);

	/** we use rulim to break out of the loop when we run out of digits */
	do {
		result *= 10;
		result += *(*buf)++ - '0';
		rulim /= 10;
	} while ((result * 10 <= ulim) && rulim && **buf >= '0' && **buf <= '9');

	if (result < llim || result > ulim)
		return (0);

	*dest = result;
	return (1);
}

#endif /** HAVE_STRPTIME */





#endif
