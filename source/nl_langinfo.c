#include <langinfo.h>

char *nl_langinfo(nl_item item) {
    switch (item) {
        case CODESET: return "UTF-8";
        case D_T_FMT: return "%a %b %e %H:%M:%S %Y";
        case D_FMT: return "%m/%d/%y";
        case T_FMT: return "%H:%M:%S";
        case T_FMT_AMPM: return "%I:%M:%S %p";
        case AM_STR: return "AM";
        case PM_STR: return "PM";
        
        case DAY_1: return "Sunday";
        case DAY_2: return "Monday";
        case DAY_3: return "Tuesday";
        case DAY_4: return "Wednesday";
        case DAY_5: return "Thursday";
        case DAY_6: return "Friday";
        case DAY_7: return "Saturday";
        
        case ABDAY_1: return "Sun";
        case ABDAY_2: return "Mon";
        case ABDAY_3: return "Tue";
        case ABDAY_4: return "Wed";
        case ABDAY_5: return "Thu";
        case ABDAY_6: return "Fri";
        case ABDAY_7: return "Sat";
        
        case MON_1: return "January";
        case MON_2: return "February";
        case MON_3: return "March";
        case MON_4: return "April";
        case MON_5: return "May";
        case MON_6: return "June";
        case MON_7: return "July";
        case MON_8: return "August";
        case MON_9: return "September";
        case MON_10: return "October";
        case MON_11: return "November";
        case MON_12: return "December";
        
        case ABMON_1: return "Jan";
        case ABMON_2: return "Feb";
        case ABMON_3: return "Mar";
        case ABMON_4: return "Apr";
        case ABMON_5: return "May";
        case ABMON_6: return "Jun";
        case ABMON_7: return "Jul";
        case ABMON_8: return "Aug";
        case ABMON_9: return "Sep";
        case ABMON_10: return "Oct";
        case ABMON_11: return "Nov";
        case ABMON_12: return "Dec";
        
        case ALTMON_1: return "January";
        case ALTMON_2: return "February";
        case ALTMON_3: return "March";
        case ALTMON_4: return "April";
        case ALTMON_5: return "May";
        case ALTMON_6: return "June";
        case ALTMON_7: return "July";
        case ALTMON_8: return "August";
        case ALTMON_9: return "September";
        case ALTMON_10: return "October";
        case ALTMON_11: return "November";
        case ALTMON_12: return "December";
        
        case ABALTMON_1: return "Jan";
        case ABALTMON_2: return "Feb";
        case ABALTMON_3: return "Mar";
        case ABALTMON_4: return "Apr";
        case ABALTMON_5: return "May";
        case ABALTMON_6: return "Jun";
        case ABALTMON_7: return "Jul";
        case ABALTMON_8: return "Aug";
        case ABALTMON_9: return "Sep";
        case ABALTMON_10: return "Oct";
        case ABALTMON_11: return "Nov";
        case ABALTMON_12: return "Dec";
        
        case ERA: return "";
        case ERA_D_FMT: return "";
        case ERA_D_T_FMT: return "";
        case ERA_T_FMT: return "";
        case ALT_DIGITS: return "";
        
        case RADIXCHAR: return ".";
        case THOUSEP: return ",";
        
        case YESEXPR: return "^[yY]";
        case NOEXPR: return "^[nN]";
        
        case CRNCYSTR: return "$";
        
        default: return "";
    }
}