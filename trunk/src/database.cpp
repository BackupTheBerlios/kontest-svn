#include "database.h"
#include <QDebug>


DataBase::DataBase(){}

DataBase::~DataBase(){}


bool DataBase::createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    //db.setDatabaseName(":memory:"); // 2m 07s
    db.setDatabaseName("logbook.dat");
    //rc = sqlite3_open(":memory:", &db);
    if (!db.open()) {
        QMessageBox::warning(0, QObject::tr("Database Error"),
                             db.lastError().text());
        //qDebug() << "DataBase::createConnection: DB creation ERROR"  << endl;
        return false;
    }
    //qDebug() << "DataBase::createConnection: DB creation OK"  << endl;
    return true;
}


bool DataBase::createDataBase()
{
    //http://www.sqlite.org/
    //http://www.sqlite.org/datatype3.html
    qDebug() << "DataBase::createData"  << endl;

    QSqlQuery query;
    query.exec("DROP TABLE log");
    query.exec("DROP TABLE band");
    query.exec("DROP TABLE mode");
    query.exec("DROP TABLE mode");
    query.exec("DROP TABLE prefixesofentity");
    query.exec("DROP TABLE continent");
    query.exec("DROP TABLE entity");

    query.exec("CREATE TABLE band ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "lower VARCHAR(7) NOT NULL, "
                "upper VARCHAR(7) NOT NULL, "
                "name VARCHAR(40) NOT NULL)");


    query.exec("CREATE TABLE mode ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "name VARCHAR(40) NOT NULL)");

      query.exec("CREATE TABLE log ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "call VARCHAR(40) NOT NULL, "
               "bandid INTEGER NOT NULL, "
               "modeid INTEGER NOT NULL, "
               "qso_date VARCHAR(10) NOT NULL, "
               "qso_date_off VARCHAR(10), "
                "time_off VARCHAR(8), "
                "time_on VARCHAR(8) NOT NULL, "
               "srx VARCHAR(10), "
               "stx VARCHAR(10), "
               "points INTEGER,"
               "multiplier INTEGER,"
               "cqz INTEGER,"
               "ituz INTEGER,"
               "dxcc INTEGER,"
               "address VARCHAR, "
               "age INTEGER, "
               "cnty VARCHAR, "
               "comment VARCHAR, "
               "a_index INTEGER, "
               "ant_az INTEGER, "
               "ant_el INTEGER, "
               "ant_path INTEGER, "
               "arrl_sect INTEGER, "
               "band_rx INTEGER, "
               "checkcontest VARCHAR, "
               "class VARCHAR, "
               "contacted_op VARCHAR(40), "
               "contest_id VARCHAR, "
               "country VARCHAR, "
               "credit_submitted VARCHAR, "
               "credit_granted VARCHAR, "
               "distance INTEGER, "
               "email VARCHAR, "
               "eq_call VARCHAR, "
               "eqsl_qslrdate VARCHAR(10), "
               "eqsl_qslsdate VARCHAR(10), "
               "eqsl_qsl_rcvd VARCHAR(1), "
               "eqsl_qsl_sent VARCHAR(1), "
               "force_init INTEGER, "
               "freq INTEGER, "
               "freq_rx INTEGER, "
               "gridsquare VARCHAR, "
               "iota VARCHAR(6), "
               "iota_island_id VARCHAR, "
               "k_index INTEGER, "
               "lat VARCHAR(11), "
               "lon VARCHAR(11), "
               "lotw_qslrdate VARCHAR(10), "
               "lotw_qslsdate VARCHAR(10), "
               "lotw_qsl_rcvd VARCHAR(1), "
               "lotw_qsl_sent VARCHAR(1), "
               "max_bursts INTEGER, "
               "ms_shower VARCHAR, "
               "my_city VARCHAR, "
               "my_cnty VARCHAR, "
               "my_country INTEGER, "
               "my_cq_zone INTEGER, "
               "my_gridsquare VARCHAR, "
               "my_iota VARCHAR(6), "
               "my_iota_island_id VARCHAR, "
               "my_lat VARCHAR(11), "
               "my_lon VARCHAR(11), "
               "my_name VARCHAR, "
               "my_operator VARCHAR, "
               "my_rig VARCHAR, "
               "my_sig VARCHAR, "
               "my_sig_info VARCHAR, "
               "my_state VARCHAR, "
               "my_street VARCHAR, "
               "name VARCHAR, "
               "notes VARCHAR, "
               "nr_bursts INTEGER, "
               "nr_pings INTEGER, "
               "operator VARCHAR, "
               "owner_callsign VARCHAR, "
               "pfx VARCHAR, "
               "precedence VARCHAR, "
               "prop_mode VARCHAR(8), "
               "public_key VARCHAR, "
               "QSLMSG VARCHAR, "
               "qslrdate VARCHAR(10), "
               "qslsdate VARCHAR(10), "
               "qsl_rcvd VARCHAR(1), "
               "qsl_sent VARCHAR(1), "
               "qsl_rcvd_via VARCHAR(1), "
               "qsl_sent_via VARCHAR(1), "
               "qsl_via VARCHAR, "
               "qso_complete VARCHAR(1), "
               "qso_random INTEGER, "
               "qth VARCHAR, "
               "rst_sent VARCHAR, "
               "rst_rcvd VARCHAR, "
               "rx_power INTEGER, "
               "sat_mode VARCHAR, "
               "sat_name VARCHAR, "
               "sfi INTEGER, "
               "sig VARCHAR, "
               "sig_info VARCHAR, "
               "srx_string VARCHAR, "
               "stx_string VARCHAR, "
               "state VARCHAR, "
               "station_callsign VARCHAR, "
               "swl INTEGER, "
               "ten_ten INTEGER, "
               "tx_pwr INTEGER, "
               "web VARCHAR, "
               "FOREIGN KEY (qso_complete) REFERENCES qso_complete_enumeration, "
               "FOREIGN KEY (qsl_rcvd_via) REFERENCES qsl_via_enumeration, "
               "FOREIGN KEY (qsl_sent_via) REFERENCES qsl_via_enumeration, "
               "FOREIGN KEY (qsl_rcvd) REFERENCES qsl_rec_status, "
               "FOREIGN KEY (qsl_sent) REFERENCES qsl_sent_status, "
               "FOREIGN KEY (prop_mode) REFERENCES prop_mode_emumeration, "
               "FOREIGN KEY (my_country) REFERENCES entity, "
               "FOREIGN KEY (lotw_qsl_rcvd) REFERENCES qsl_rec_status, "
               "FOREIGN KEY (lotw_qsl_sent) REFERENCES qsl_sent_status, "
               "FOREIGN KEY (eqsl_qsl_rcvd) REFERENCES qsl_rec_status, "
               "FOREIGN KEY (eqsl_qsl_sent) REFERENCES qsl_sent_status, "
               "FOREIGN KEY (credit_submitted) REFERENCES award_enumeration, "
               "FOREIGN KEY (credit_granted) REFERENCES award_enumeration, "
               "FOREIGN KEY (country) REFERENCES entity, "
               "FOREIGN KEY (ant_path) REFERENCES ant_path_enumeration, "
               "FOREIGN KEY (arrl_sect) REFERENCES arrl_sect_enumeration, "
               "FOREIGN KEY (band_rx) REFERENCES band, "
               "FOREIGN KEY (modeid) REFERENCES mode, "
               "FOREIGN KEY (dxcc) REFERENCES entity, "
               "FOREIGN KEY (bandid) REFERENCES band)");

      //DATE YYYY-MM-DD
      //TIME HHmmss
      //http://www.sqlite.org/lang_datefunc.html

      query.exec("CREATE TABLE continent ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "shortname VARCHAR(2) NOT NULL, "
                 "name VARCHAR(15) NOT NULL)");

      query.exec("CREATE TABLE ant_path_enumeration ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "shortname VARCHAR(1) NOT NULL, "
                 "name VARCHAR(15) NOT NULL)");

      query.exec("CREATE TABLE arrl_sect_enumeration ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "shortname VARCHAR(2) NOT NULL, "
                 "name VARCHAR(30) NOT NULL)");

      query.exec("CREATE TABLE qso_complete_enumeration ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "shortname VARCHAR(3) NOT NULL, "
                 "name VARCHAR(10) NOT NULL)");

      query.exec("CREATE TABLE contest ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "shortname VARCHAR(20) NOT NULL, "
                 "name VARCHAR(40) NOT NULL)");

      query.exec("CREATE TABLE award_enumeration ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "name VARCHAR(15) NOT NULL)");

      query.exec("CREATE TABLE entity ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "name VARCHAR(40) NOT NULL,"
                 "cqz INTEGER NOT NULL, "
                 "ituz INTEGER NOT NULL, "
                 "continent INTEGER NOT NULL, "
                 "latitude REAL NOT NULL, "
                 "longitude REAL NOT NULL, "
                 "utc INTEGER NOT NULL, "
                 "arrlid INTEGER NOT NULL, "
                 "mainprefix VARCHAR(15) NOT NULL, "
                 "deleted INTEGER, "
                 "FOREIGN KEY (continent) REFERENCES continent)");
//TODO: To add some columns in this the table to mark if worked/confirmed/band/Mode

      query.exec("CREATE TABLE prefixesofentity ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "prefix VARCHAR(15) NOT NULL,"
                 "dxcc INTEGER NOT NULL,"
                 "cqz INTEGER NOT NULL,"
                 "ituz INTEGER NOT NULL,"
                 "FOREIGN KEY (dxcc) REFERENCES entity)");

      query.exec("CREATE TABLE qsl_rec_status ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "shortname VARCHAR(1) NOT NULL, "
                 "name VARCHAR(15) NOT NULL)");

      query.exec("CREATE TABLE qsl_sent_status ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "shortname VARCHAR(1) NOT NULL, "
                 "name VARCHAR(15) NOT NULL)");

      query.exec("CREATE TABLE qsl_via ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "shortname VARCHAR(1) NOT NULL, "
                 "name VARCHAR(15) NOT NULL)");

      query.exec("CREATE TABLE prop_mode_enumeration ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "shortname VARCHAR(8) NOT NULL, "
                 "name VARCHAR(55) NOT NULL)");



query.exec("INSERT INTO qsl_sent_status (shortname, name) VALUES ('Y', 'Yes')");
query.exec("INSERT INTO qsl_sent_status (shortname, name) VALUES ('N', 'No')");
query.exec("INSERT INTO qsl_sent_status (shortname, name) VALUES ('R', 'Requested')");
query.exec("INSERT INTO qsl_sent_status (shortname, name) VALUES ('Q', 'Queued')");
query.exec("INSERT INTO qsl_sent_status (shortname, name) VALUES ('I', 'Ignore/Invalid')");

query.exec("INSERT INTO qsl_rec_status (shortname, name) VALUES ('Y', 'Yes')");
query.exec("INSERT INTO qsl_rec_status (shortname, name) VALUES ('N', 'No')");
query.exec("INSERT INTO qsl_rec_status (shortname, name) VALUES ('R', 'Requested')");
query.exec("INSERT INTO qsl_rec_status (shortname, name) VALUES ('I', 'Ignore/Invalid')");
query.exec("INSERT INTO qsl_rec_status (shortname, name) VALUES ('V', 'Validated')");

      query.exec("INSERT INTO continent (shortname, name) VALUES ('AF', 'Africa')");
      query.exec("INSERT INTO continent (shortname, name) VALUES ('AS', 'Asia')");
      query.exec("INSERT INTO continent (shortname, name) VALUES ('EU', 'Europe')");
      query.exec("INSERT INTO continent (shortname, name) VALUES ('NA', 'North America')");
      query.exec("INSERT INTO continent (shortname, name) VALUES ('SA', 'South America')");
      query.exec("INSERT INTO continent (shortname, name) VALUES ('OC', 'Oceania')");

      //To add a band, just create another line:
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('1mm', '241000', '250000')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('2mm', '142000', '149000')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('2.5mm', '119980', '120020')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('4mm', '75500', '81000')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('6mm', '47000', '47200')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('1.25CM', '24000', '24250')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('3CM', '10000', '10500')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('6CM', '5650', '5925')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('9CM', '3300', '3500')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('23CM', '1240', '1300')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('33CM', '902', '928')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('70CM', '420', '450')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('1.25M', '222', '225')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('2M', '144', '148')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('4M', '70', '71')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('6M', '50', '54')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('10M', '28.0', '29.7')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('12M', '24.89', '24.99')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('15M', '21.0', '21.45')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('17M', '18.068', '18.168')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('20M', '14.0', '14.35')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('30M', '10.0', '10.15')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('40M', '7.0', '7.3')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('60M', '5.102', '5.404')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('80M', '3.5', '4.0')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('160M', '1.8', '2.0')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('560M', '0.501', '0.504')");
      query.exec("INSERT INTO band (name, lower, upper) VALUES ('2190M', '0.136', '0.137')");


//To add a mode, just create another line:
      query.exec("INSERT INTO mode (name) VALUES ('SSB')");
      query.exec("INSERT INTO mode (name) VALUES ('CW')");
      query.exec("INSERT INTO mode (name) VALUES ('RTTY')");
//      query.exec("INSERT INTO mode (name) VALUES ('PSK31')");

query.exec("INSERT INTO ant_path_enumeration (shortname, name) VALUES ('G', 'GrayLine')");
query.exec("INSERT INTO ant_path_enumeration (shortname, name) VALUES ('O', 'Other')");
query.exec("INSERT INTO ant_path_enumeration (shortname, name) VALUES ('S', 'ShortPath')");
query.exec("INSERT INTO ant_path_enumeration (shortname, name) VALUES ('L', 'LongPath')");

query.exec("INSERT INTO arrl_sect_enumeration (shortname, name) VALUES ('AL', 'Alabama')");

query.exec("INSERT INTO contest (shortname, name) VALUES ('CQ-WW-SSB', 'CQ WW DX Contest(SSB)')");
query.exec("INSERT INTO contest (shortname, name) VALUES ('CQ-WW-CW', 'CQ WW DX Contest(CW)')");
query.exec("INSERT INTO contest (shortname, name) VALUES ('CQ-WPX-SSB', 'CQ WW WPX Contest (SSB)')");
query.exec("INSERT INTO contest (shortname, name) VALUES ('CQ-WPX-CW', 'CQ WW CW Contest (CW)')");
query.exec("INSERT INTO contest (shortname, name) VALUES ('CQ-WW-RTTY', 'CQ/RJ WW RTTY DX Contest')");

query.exec("INSERT INTO award_enumeration (name) VALUES ('AJA')");
query.exec("INSERT INTO award_enumeration (name) VALUES ('CQDX')");
query.exec("INSERT INTO award_enumeration (name) VALUES (CQDXFIELD')");
query.exec("INSERT INTO award_enumeration (name) VALUES (DXCC')");

query.exec("INSERT INTO prop_mode_enumeration (shortname, name) VALUES ('AUR', 'Aurora')");
query.exec("INSERT INTO prop_mode_enumeration (shortname, name) VALUES ('AUE', 'Aurora-E')");
query.exec("INSERT INTO prop_mode_enumeration (shortname, name) VALUES ('BS', 'Back scatter')");

query.exec("INSERT INTO qsl_via (shortname, name) VALUES ('B', 'Bureau')");
query.exec("INSERT INTO qsl_via (shortname, name) VALUES ('D', 'Direct')");
query.exec("INSERT INTO qsl_via (shortname, name) VALUES ('E', 'Electronic')");
query.exec("INSERT INTO qsl_via (shortname, name) VALUES ('M', 'Manager')");

query.exec("INSERT INTO qso_complete_enumeration (shortname, name) VALUES ('Y', 'Yes')");
query.exec("INSERT INTO qso_complete_enumeration (shortname, name) VALUES ('N', 'No')");
query.exec("INSERT INTO qso_complete_enumeration (shortname, name) VALUES ('NIL', 'Not heard')");
query.exec("INSERT INTO qso_complete_enumeration (shortname, name) VALUES ('?', 'Uncertain')");

return true;


}
