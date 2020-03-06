# cron_parser

**cron_parser** - is a command line utility that converts cron expression into readable text format. It expects **6** required arguments:
*cron_parser [minutes] [hours] [days] [months] [weekdays] [command]*
**What is supported:**
**\*** - generates all possible values for time field;
**/** - allows to specify step of generation; e.g. 1/2 - generates uneven numbers starting from 1(1,3,5,7...);
**-** - range of values. e.g. 1-3 generates numbers from 1 to 3(1,2,3).
**,** - allows to specify particular values or expressions. e.g. 1,2,10,13-15.
**JUN-AUG** - months can be specified as strings and not only numbers. For instance JUN-AUG will be equal to 6-8.
**MON-FRI** - weekdays can be specified as strings and not only numbers. For instance FRI-SUN will be equal to 5-7.
**How to use examples:**
*cron_parser 1/1 1 1 1 1 foo
cron_parser 1,15-22,7,* 1 1 1 1 foo
cron_parser 1-27 1 1 1 1 foo*