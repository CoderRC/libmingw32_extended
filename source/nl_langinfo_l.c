#include <langinfo.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char *name;
    int category;
} locale_data_t;

static char *get_locale_string(nl_item item, const char *locale_name) {
    if (!locale_name || strcmp(locale_name, "C") == 0 || strcmp(locale_name, "POSIX") == 0) {
        return nl_langinfo(item);
    }
    
    // Mandarin Chinese (zh)
    if (strncmp(locale_name, "zh", 2) == 0) {
        switch (item) {
            case DAY_1: return "星期日";
            case DAY_2: return "星期一";
            case DAY_3: return "星期二";
            case DAY_4: return "星期三";
            case DAY_5: return "星期四";
            case DAY_6: return "星期五";
            case DAY_7: return "星期六";
            case ABDAY_1: return "日";
            case ABDAY_2: return "一";
            case ABDAY_3: return "二";
            case ABDAY_4: return "三";
            case ABDAY_5: return "四";
            case ABDAY_6: return "五";
            case ABDAY_7: return "六";
            case MON_1: return "一月";
            case MON_2: return "二月";
            case MON_3: return "三月";
            case MON_4: return "四月";
            case MON_5: return "五月";
            case MON_6: return "六月";
            case MON_7: return "七月";
            case MON_8: return "八月";
            case MON_9: return "九月";
            case MON_10: return "十月";
            case MON_11: return "十一月";
            case MON_12: return "十二月";
            case RADIXCHAR: return ".";
            case THOUSEP: return ",";
            case CRNCYSTR: return "¥";
        }
    }
    
    // Hindi (hi)
    if (strncmp(locale_name, "hi", 2) == 0) {
        switch (item) {
            case DAY_1: return "रविवार";
            case DAY_2: return "सोमवार";
            case DAY_3: return "मंगलवार";
            case DAY_4: return "बुधवार";
            case DAY_5: return "गुरुवार";
            case DAY_6: return "शुक्रवार";
            case DAY_7: return "शनिवार";
            case MON_1: return "जनवरी";
            case MON_2: return "फरवरी";
            case MON_3: return "मार्च";
            case MON_4: return "अप्रैल";
            case MON_5: return "मई";
            case MON_6: return "जून";
            case MON_7: return "जुलाई";
            case MON_8: return "अगस्त";
            case MON_9: return "सितंबर";
            case MON_10: return "अक्टूबर";
            case MON_11: return "नवंबर";
            case MON_12: return "दिसंबर";
            case RADIXCHAR: return ".";
            case THOUSEP: return ",";
            case CRNCYSTR: return "₹";
        }
    }
    
    // Spanish (es)
    if (strncmp(locale_name, "es", 2) == 0) {
        switch (item) {
            case DAY_1: return "domingo";
            case DAY_2: return "lunes";
            case DAY_3: return "martes";
            case DAY_4: return "miércoles";
            case DAY_5: return "jueves";
            case DAY_6: return "viernes";
            case DAY_7: return "sábado";
            case ABDAY_1: return "dom";
            case ABDAY_2: return "lun";
            case ABDAY_3: return "mar";
            case ABDAY_4: return "mié";
            case ABDAY_5: return "jue";
            case ABDAY_6: return "vie";
            case ABDAY_7: return "sáb";
            case MON_1: return "enero";
            case MON_2: return "febrero";
            case MON_3: return "marzo";
            case MON_4: return "abril";
            case MON_5: return "mayo";
            case MON_6: return "junio";
            case MON_7: return "julio";
            case MON_8: return "agosto";
            case MON_9: return "septiembre";
            case MON_10: return "octubre";
            case MON_11: return "noviembre";
            case MON_12: return "diciembre";
            case RADIXCHAR: return ",";
            case THOUSEP: return ".";
            case CRNCYSTR: return "€";
            case YESEXPR: return "^[sS]";
            case NOEXPR: return "^[nN]";
        }
    }
    
    // French (fr)
    if (strncmp(locale_name, "fr", 2) == 0) {
        switch (item) {
            case DAY_1: return "dimanche";
            case DAY_2: return "lundi";
            case DAY_3: return "mardi";
            case DAY_4: return "mercredi";
            case DAY_5: return "jeudi";
            case DAY_6: return "vendredi";
            case DAY_7: return "samedi";
            case ABDAY_1: return "dim";
            case ABDAY_2: return "lun";
            case ABDAY_3: return "mar";
            case ABDAY_4: return "mer";
            case ABDAY_5: return "jeu";
            case ABDAY_6: return "ven";
            case ABDAY_7: return "sam";
            case MON_1: return "janvier";
            case MON_2: return "février";
            case MON_3: return "mars";
            case MON_4: return "avril";
            case MON_5: return "mai";
            case MON_6: return "juin";
            case MON_7: return "juillet";
            case MON_8: return "août";
            case MON_9: return "septembre";
            case MON_10: return "octobre";
            case MON_11: return "novembre";
            case MON_12: return "décembre";
            case RADIXCHAR: return ",";
            case THOUSEP: return " ";
            case CRNCYSTR: return "€";
            case YESEXPR: return "^[oOyY]";
            case NOEXPR: return "^[nN]";
        }
    }
    
    // Arabic (ar)
    if (strncmp(locale_name, "ar", 2) == 0) {
        switch (item) {
            case DAY_1: return "الأحد";
            case DAY_2: return "الاثنين";
            case DAY_3: return "الثلاثاء";
            case DAY_4: return "الأربعاء";
            case DAY_5: return "الخميس";
            case DAY_6: return "الجمعة";
            case DAY_7: return "السبت";
            case MON_1: return "يناير";
            case MON_2: return "فبراير";
            case MON_3: return "مارس";
            case MON_4: return "أبريل";
            case MON_5: return "مايو";
            case MON_6: return "يونيو";
            case MON_7: return "يوليو";
            case MON_8: return "أغسطس";
            case MON_9: return "سبتمبر";
            case MON_10: return "أكتوبر";
            case MON_11: return "نوفمبر";
            case MON_12: return "ديسمبر";
            case RADIXCHAR: return ".";
            case THOUSEP: return ",";
        }
    }
    
    // Bengali (bn)
    if (strncmp(locale_name, "bn", 2) == 0) {
        switch (item) {
            case DAY_1: return "রবিবার";
            case DAY_2: return "সোমবার";
            case DAY_3: return "মঙ্গলবার";
            case DAY_4: return "বুধবার";
            case DAY_5: return "বৃহস্পতিবার";
            case DAY_6: return "শুক্রবার";
            case DAY_7: return "শনিবার";
            case MON_1: return "জানুয়ারি";
            case MON_2: return "ফেব্রুয়ারি";
            case MON_3: return "মার্চ";
            case MON_4: return "এপ্রিল";
            case MON_5: return "মে";
            case MON_6: return "জুন";
            case MON_7: return "জুলাই";
            case MON_8: return "আগস্ট";
            case MON_9: return "সেপ্টেম্বর";
            case MON_10: return "অক্টোবর";
            case MON_11: return "নভেম্বর";
            case MON_12: return "ডিসেম্বর";
            case RADIXCHAR: return ".";
            case THOUSEP: return ",";
            case CRNCYSTR: return "৳";
        }
    }
    
    // Portuguese (pt)
    if (strncmp(locale_name, "pt", 2) == 0) {
        switch (item) {
            case DAY_1: return "domingo";
            case DAY_2: return "segunda-feira";
            case DAY_3: return "terça-feira";
            case DAY_4: return "quarta-feira";
            case DAY_5: return "quinta-feira";
            case DAY_6: return "sexta-feira";
            case DAY_7: return "sábado";
            case ABDAY_1: return "dom";
            case ABDAY_2: return "seg";
            case ABDAY_3: return "ter";
            case ABDAY_4: return "qua";
            case ABDAY_5: return "qui";
            case ABDAY_6: return "sex";
            case ABDAY_7: return "sáb";
            case MON_1: return "janeiro";
            case MON_2: return "fevereiro";
            case MON_3: return "março";
            case MON_4: return "abril";
            case MON_5: return "maio";
            case MON_6: return "junho";
            case MON_7: return "julho";
            case MON_8: return "agosto";
            case MON_9: return "setembro";
            case MON_10: return "outubro";
            case MON_11: return "novembro";
            case MON_12: return "dezembro";
            case RADIXCHAR: return ",";
            case THOUSEP: return ".";
            case CRNCYSTR: return "R$";
            case YESEXPR: return "^[sS]";
            case NOEXPR: return "^[nN]";
        }
    }
    
    // Russian (ru)
    if (strncmp(locale_name, "ru", 2) == 0) {
        switch (item) {
            case DAY_1: return "воскресенье";
            case DAY_2: return "понедельник";
            case DAY_3: return "вторник";
            case DAY_4: return "среда";
            case DAY_5: return "четверг";
            case DAY_6: return "пятница";
            case DAY_7: return "суббота";
            case ABDAY_1: return "вс";
            case ABDAY_2: return "пн";
            case ABDAY_3: return "вт";
            case ABDAY_4: return "ср";
            case ABDAY_5: return "чт";
            case ABDAY_6: return "пт";
            case ABDAY_7: return "сб";
            case MON_1: return "январь";
            case MON_2: return "февраль";
            case MON_3: return "март";
            case MON_4: return "апрель";
            case MON_5: return "май";
            case MON_6: return "июнь";
            case MON_7: return "июль";
            case MON_8: return "август";
            case MON_9: return "сентябрь";
            case MON_10: return "октябрь";
            case MON_11: return "ноябрь";
            case MON_12: return "декабрь";
            case RADIXCHAR: return ",";
            case THOUSEP: return " ";
            case CRNCYSTR: return "₽";
            case YESEXPR: return "^[дД]";
            case NOEXPR: return "^[нН]";
        }
    }
    
    // Japanese (ja)
    if (strncmp(locale_name, "ja", 2) == 0) {
        switch (item) {
            case DAY_1: return "日曜日";
            case DAY_2: return "月曜日";
            case DAY_3: return "火曜日";
            case DAY_4: return "水曜日";
            case DAY_5: return "木曜日";
            case DAY_6: return "金曜日";
            case DAY_7: return "土曜日";
            case ABDAY_1: return "日";
            case ABDAY_2: return "月";
            case ABDAY_3: return "火";
            case ABDAY_4: return "水";
            case ABDAY_5: return "木";
            case ABDAY_6: return "金";
            case ABDAY_7: return "土";
            case MON_1: return "1月";
            case MON_2: return "2月";
            case MON_3: return "3月";
            case MON_4: return "4月";
            case MON_5: return "5月";
            case MON_6: return "6月";
            case MON_7: return "7月";
            case MON_8: return "8月";
            case MON_9: return "9月";
            case MON_10: return "10月";
            case MON_11: return "11月";
            case MON_12: return "12月";
            case RADIXCHAR: return ".";
            case THOUSEP: return ",";
            case CRNCYSTR: return "¥";
        }
    }
    
    // Default to English
    return nl_langinfo(item);
}

char *nl_langinfo_l(nl_item item, locale_t loc) {
    if (loc == NULL) {
        return nl_langinfo(item);
    }
    
    // Cast locale_t to our internal structure
    locale_data_t *locale_data = (locale_data_t *)loc;
    
    return get_locale_string(item, locale_data->name);
}