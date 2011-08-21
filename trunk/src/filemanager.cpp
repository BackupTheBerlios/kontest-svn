#include "filemanager.h"
#include <QDebug>

FileManager::FileManager()
{
qDebug() << "FileManager::FileManager()" << endl;




}

FileManager::FileManager(const QString _kontestDir)
{
    //qDebug() << "FileManager::FileManager(): Dir" << _kontestDir << endl;
    kontestDir = _kontestDir;

}

FileManager::~FileManager()
{

}


bool FileManager::adifReadLog(const QString& tfileName)
{
    qDebug() << "FileManager::adifReadLog:" << tfileName << endl;

    QStringList fields, qsToPass, qsAux;
    QString fileName = tfileName;
    QString line;
    bool inHeader = true;
    bool EOR = false;
    bool noMoreQso = false;
    qint64 pos; //Position in the file
    int i; //Aunxiliar variable
    int qsos = 0;
    int numberOfQsos = 0;

    QFile file( fileName );
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "FileManager::adifReadLog File not found" << fileName << endl;
        return false;
    }
    pos = file.pos();

    while ( !file.atEnd()   )
    {
        line = file.readLine();
        numberOfQsos = numberOfQsos + line.count("EOR>");
    }
    qDebug() << "FileManager::adifReadLog QSOs found: " << QString::number(numberOfQsos) << endl;

    file.seek(pos);

    /*

    Optional header information may be included before the actual data in the file.
    To include optional header info, the first character of the file must be something other than <.
    Any amount of header info of any value except <eoh> may be included. The header info must be
    terminated with <eoh>. Any number of characters of any value except < may follow <eoh>.
    The first < after <eoh> is the start of the first field of the first data record in the file.

    */

    int progressBarPosition = 0;
    QProgressDialog progress("Reading ADIF file...", "Abort reading", 0, numberOfQsos);
    //progress.setWindowModality(Qt::ApplicationModal);


    line = file.readLine().trimmed().toUpper();


    if ( (!(line.startsWith('<'))) && (inHeader) )
    { // The file has a header

        line.clear(); // We should finish the if with real data in "line" or a clear one.

        while ( inHeader )
        {
            line = file.readLine().trimmed().toUpper();
            if (line.contains("<EOH>"))
            {
                inHeader = false;
                line.clear(); //TODO: Maybe this clearing deletes a line that may have data...
            }
        }
    }
    else
    { // The file does not have any header.
      // Reading the first QSO...
        /*
          Cases:
            1.- One big line with several QSO
            2.- One QSO uses several lines.
            3.- Last line of one QSO includes data of the next one
        */
    }

    while ( (!file.atEnd() ) || (!noMoreQso) )
    {
        line.append(file.readLine().trimmed().toUpper());
        //qDebug() << "FileManager::adifReadLog:" << line << endl;
        fields << line.split("<", QString::SkipEmptyParts);
        line.clear();


        while (fields.contains("EOR>"))
        {
            noMoreQso = false;
            i = 0;
            EOR = false;
            while ( (!EOR) && (!fields.isEmpty()) )
            {
                qsToPass << (fields.takeFirst()).trimmed();

                if ( (qsToPass.last() ).contains("EOR>") )
                {
                    processQsoReadingADIF(qsToPass);
                    qsos++;
                    qDebug() << "FileManager::adifReadLog:" << QString::number(qsos) << "/" << QString::number(numberOfQsos)<< endl;

                    //progressBar->setValue(qsos);
                    qsToPass.clear();
                    EOR = true;
                }


            }
            noMoreQso = true;
        }

    }
    return true;

}

bool FileManager::checkADIFValidFormat(const QStringList _qs)
{
    QStringList qs = _qs;

    if (qs.count()!= 2)
    {
        qDebug() << "FileManager::checkADIFValidFormat-0 (not two) " << endl;
        return false;
    }

    //qDebug() << "FileManager::checkADIFValidFormat: " << qs.at(0) << endl;
    //qDebug() << "FileManager::checkADIFValidFormat: " << qs.at(1) << endl;

    int i = (qs.at(0)).count(":");
    if (i != 1)
    {
        qDebug() << "FileManager::checkADIFValidFormat-1 " << endl;
        return false;
    }
    i = (qs.at(0)).indexOf(":");
    i = (qs.at(0)).length() - i -1;

    //qDebug() << "i = " << QString::number(i) << "/" << qs.at(0) << endl;

    int len = ( (qs.at(0)).right(i)).toInt();
    //qDebug() << "len = " << QString::number(len) << endl;

    if ( (qs.at(1)).length() != len )
    {
        qDebug() << "FileManager::checkADIFValidFormat-2: " << (qs.at(1)) << " - " << QString::number((qs.at(1)).length()) << "/" << QString::number(len) << endl;
        return false;
    }

    if (len <= 0)
    {
        qDebug() << "FileManager::checkADIFValidFormat-3 " << endl;
        return false;
    }

    //qDebug() << "FileManager::checkADIFValidFormat-4 (true)" << endl;

    return true;


}

bool FileManager::processQsoReadingADIF(const QStringList _line)
{
    QDate date;
    QTime time;
    QStringList qs = _line;
    QStringList oneField;
    QString field, data;
    QSqlQuery query;
    QString queryString, stringFields, stringData;
    bool ret;
    int lenght = 0;

    //qDebug() << "FileManager::processQsoReadingADIF" << QString::number(qs.size()) << endl;


    QString str;
    foreach (str, qs)
    {

        if ( !( (str.contains(":")) && (str.contains(">")) ) )
        {

        }
        else
        {
            oneField = str.split(">", QString::SkipEmptyParts);


        if (checkADIFValidFormat(oneField))
        {
            field = oneField.at(0);
            data = oneField.at(1);

            lenght = field.indexOf(":");
            field = field.left(lenght);

            if (field == "CALL")
            {
                stringFields = stringFields + ", call";
                stringData = stringData + ", '" + data + "'";
                qDebug() << "FileManager::processQsoReadingADIF" << data << endl;
            }
            else if (field == "QSO_DATE")
            {
                stringFields = stringFields + ", qso_date";
                stringData = stringData + ", '" + (date.fromString(data, "yyyyMMdd")).toString("yyyy/MM/dd") + "'";
                qDebug() << "FileManager::processQsoReadingADIF-Date: " << data << endl;
            }
            else if (field == "QSO_DATE_OFF")
            {
                stringFields = stringFields + ", qso_date_off";
                stringData = stringData + ", '" + (date.fromString(data, "yyyyMMdd")).toString("yyyy/MM/dd") + "'";
            }

            else if (field == "BAND")
            {
                queryString = QString("SELECT id FROM band WHERE name ='%1'").arg(data);
                query.exec(queryString);
                query.next();

                stringFields = stringFields + ", bandid";
                stringData = stringData + ", '" + query.value(0).toString() + "'";
                //qDebug() << "FileManager::processQsoReadingADIF-Band: " << data << endl;
            }
            else if (field == "BAND_RX")
            {
                queryString = QString("SELECT id FROM band WHERE name ='%1'").arg(data);
                query.exec(queryString);
                query.next();

                stringFields = stringFields + ", band_rx";
                stringData = stringData + ", '" + query.value(0).toString() + "'";
            }

            else if (field == "MODE")
            {
                queryString = QString("SELECT id FROM mode WHERE name ='%1'").arg(data);
                query.exec(queryString);
                query.next();

                stringFields = stringFields + ", modeid";
                stringData = stringData + ", '" + query.value(0).toString() + "'";
                //qDebug() << "FileManager::processQsoReadingADIF-Mode: " << data << endl;
            }
            else if (field == "SRX")
            {
                stringFields = stringFields + ", srx";
                stringData = stringData + ", '" + data + "'";
                //qDebug() << "FileManager::processQsoReadingADIF-srx: " << data << endl;
            }
            else if (field == "STX")
            {
                stringFields = stringFields + ", stx";
                stringData = stringData + ", '" + data + "'";
                //qDebug() << "FileManager::processQsoReadingADIF-stx: " << data << endl;
            }
            else if (field == "TIME_ON")
            {
                //QString ttime = (timeEdit->time()).toString("hh:mm:ss");
                stringFields = stringFields + ", time_on";
                if (data.length() == 4)
                {
                    data = data + "00";
                }
                stringData = stringData + ", '" + (time.fromString(data,"hhmmss")).toString("hh:mm:ss") + "'";
                qDebug() << "FileManager::processQsoReadingADIF-time_on: " << data << endl;

            }
            else if (field == "TIME_OFF")
            {
                //QString ttime = (timeEdit->time()).toString("hh:mm:ss");

                stringFields = stringFields + ", time_off";
                if (data.length() == 4)
                {
                    data = data + "00";
                }

                stringData = stringData + ", '" + (time.fromString(data,"hhmmss")).toString("hh:mm:ss") + "'";
            }

            else if (field == "srx_string")
            {
                stringFields = stringFields + ", srx_string";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "stx_string")
            {
                stringFields = stringFields + ", stx_string";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "CQZ")
            {
                stringFields = stringFields + ", cqz";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "ITUZ")
            {
                stringFields = stringFields + ", ituz";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "DXCC")
            {
                stringFields = stringFields + ", dxcc";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "ADDRESS")
            {
                stringFields = stringFields + ", address";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "AGE")
            {
                stringFields = stringFields + ", age";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "CNTY")
            {
                stringFields = stringFields + ", ";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "cnty")
            {
                stringFields = stringFields + ", ";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "COMMENT")
            {
                stringFields = stringFields + ", comment";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "A_INDEX")
            {
                stringFields = stringFields + ", a_index";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "ANT_AZ")
            {
                stringFields = stringFields + ", ant_az";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "ANT_PATH")
            {
                stringFields = stringFields + ", ant_path";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "ARRL_SECT")
            {
                stringFields = stringFields + ", arrl_sect";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "CHECKCONTEST")
            {
                stringFields = stringFields + ", checkcontest";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "CLASS")
            {
                stringFields = stringFields + ", class";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "CONTACTED_OP")
            {
                stringFields = stringFields + ", contacted_op";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "CONTEST_ID")
            {
                stringFields = stringFields + ", contest_id";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "COUNTRY")
            {
                stringFields = stringFields + ", country";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "CREDIT_SUBMITTED")
            {
                stringFields = stringFields + ", credit_submitted";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "CREDIT_GRANTED")
            {
                stringFields = stringFields + ", credit_granted";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "DISTANCE")
            {
                stringFields = stringFields + ", distance";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "eq_call")
            {
                stringFields = stringFields + ", EQ_CALL";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "EMAIL")
            {
                stringFields = stringFields + ", email";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "EQSL_QSLRDATE")
            {
                stringFields = stringFields + ", eqsl_qslrdate";
                stringData = stringData + ", '" + (date.fromString(data, "yyyyMMdd")).toString("yyyy/MM/dd") + "'";
                //stringData = stringData + ", '" + data + "'";
            }
            else if (field == "EQSL_QSLSDATE")
            {
                stringFields = stringFields + ", eqsl_qslsdate";
                stringData = stringData + ", '" + (date.fromString(data, "yyyyMMdd")).toString("yyyy/MM/dd") + "'";
                //stringData = stringData + ", '" + data + "'";
            }
            else if (field == "EQSL_QSL_RCVD")
            {
                stringFields = stringFields + ", eqsl_qsl_rcvd";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "EQSL_QSL_SENT")
            {
                stringFields = stringFields + ", eqsl_qsl_sent";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "FORCE_INIT")
            {
                stringFields = stringFields + ", force_init";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "FREQ")
            {
                stringFields = stringFields + ", freq";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "FREQ_RX")
            {
                stringFields = stringFields + ", freq_rx";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "GRIDSQUARE")
            {
                stringFields = stringFields + ", gridsquare";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "MY_GRIDSQUARE")
            {
                //qDebug() << "FileManager::processQsoReadingADIF-my_gridquare: " << data << endl;
                stringFields = stringFields + ", my_gridsquare";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "IOTA")
            {
                stringFields = stringFields + ", IOTA";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "IOTA_ISLAND_ID")
            {
                stringFields = stringFields + ", iota_island_id";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "MY_IOTA")
            {
                stringFields = stringFields + ", my_iota";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "MY_IOTA_ISLAND_ID")
            {
                stringFields = stringFields + ", my_iota_island_id";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "K_INDEX")
            {
                stringFields = stringFields + ", k_index";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "LAT")
            {
                stringFields = stringFields + ", lat";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "LON")
            {
                stringFields = stringFields + ", lon";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "MY_LAT")
            {
                stringFields = stringFields + ", my_lat";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "MY_LON")
            {
                stringFields = stringFields + ", my_lon";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "LOTW_QSLRDATE")
            {
                stringFields = stringFields + ", lotw_qslrdate";
                stringData = stringData + ", '" + (date.fromString(data, "yyyyMMdd")).toString("yyyy/MM/dd") + "'";
                //stringData = stringData + ", '" + data + "'";
            }
            else if (field == "LOTW_QSLSDATE")
            {
                stringFields = stringFields + ", lotw_qslsdate";
                stringData = stringData + ", '" + (date.fromString(data, "yyyyMMdd")).toString("yyyy/MM/dd") + "'";
                //stringData = stringData + ", '" + data + "'";
            }
            else if (field == "LOTW_QSL_RCVD")
            {
                stringFields = stringFields + ", lotw_qsl_rcvd";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "LOTW_QSL_SENT")
            {
                stringFields = stringFields + ", lotw_qsl_sent";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "MAX_BURSTS")
            {
                stringFields = stringFields + ", max_bursts";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "MS_SHOWER")
            {
                stringFields = stringFields + ", ms_shower";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "MY_CITY")
            {
                stringFields = stringFields + ", my_city";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "MY_CNTY")
            {
                stringFields = stringFields + ", my_cnty";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "MY_COUNTRY")
            {
                stringFields = stringFields + ", my_country";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "MY_CQ_ZONE")
            {
                stringFields = stringFields + ", my_cq_zone";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "MY_NAME")
            {
                stringFields = stringFields + ", my_name";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "NAME")
            {
                stringFields = stringFields + ", name";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "MY_OPERATOR")
            {
                stringFields = stringFields + ", my_operator";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "OPERATOR")
            {
                //qDebug() << "FileManager::processQsoReadingADIF-operator: " << data << endl;
                stringFields = stringFields + ", operator";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "STATION_CALLSIGN")
            {
                stringFields = stringFields + ", station_callsign";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "OWNER_CALLSIGN")
            {
                stringFields = stringFields + ", owner_callsign";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "MY_RIG")
            {
                stringFields = stringFields + ", my_rig";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "MY_SIG")
            {
                stringFields = stringFields + ", my_sig";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "MY_SIG_INFO")
            {
                stringFields = stringFields + ", my_sig_info";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "MY_STATE")
            {
                stringFields = stringFields + ", my_state";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "STATE")
            {
                stringFields = stringFields + ", state";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "MY_STREET")
            {
                stringFields = stringFields + ", my_street";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "NOTES")
            {
                stringFields = stringFields + ", notes";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "NR_BURSTS")
            {
                stringFields = stringFields + ", nr_bursts";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "NR_PINGS")
            {
                stringFields = stringFields + ", nr_pings";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "PFX")
            {
                stringFields = stringFields + ", pfx";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "PRECEDENCE")
            {
                stringFields = stringFields + ", precedence";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "PROP_MODE")
            {
                stringFields = stringFields + ", prop_mode";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "PUBLIC_KEY")
            {
                stringFields = stringFields + ", public_key";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "QSLMSG")
            {
                stringFields = stringFields + ", qslmsg";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "QSLRDATE")
            {
                stringFields = stringFields + ", qslrdate";
                stringData = stringData + ", '" + (date.fromString(data, "yyyyMMdd")).toString("yyyy/MM/dd") + "'";
                //stringData = stringData + ", '" + data + "'";
            }
            else if (field == "QSLSDATE")
            {
                //qDebug() << "FileManager::processQsoReadingADIF-qslsdate: " << data << endl;
                stringFields = stringFields + ", qslsdate";
                stringData = stringData + ", '" + (date.fromString(data, "yyyyMMdd")).toString("yyyy/MM/dd") + "'";
                //stringData = stringData + ", '" + data + "'";
            }
            else if (field == "QSL_RCVD")
            {
                //qDebug() << "FileManager::processQsoReadingADIF-qsl_rcvd: " << data << endl;
                stringFields = stringFields + ", qsl_rcvd";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "QSL_SENT")
            {
                //qDebug() << "FileManager::processQsoReadingADIF-qsl_sent: " << data << endl;
                stringFields = stringFields + ", qsl_sent";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "QSL_RCVD_VIA")
            {
                stringFields = stringFields + ", qsl_rcvd_via";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "QSL_SENT_VIA")
            {
                stringFields = stringFields + ", qsl_sent_via";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "QSL_VIA")
            {
                stringFields = stringFields + ", qsl_via";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "QSO_COMPLETE")
            {
                stringFields = stringFields + ", qso_complete";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "QSO_RANDOM")
            {
                stringFields = stringFields + ", qso_random";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "QTH")
            {
                stringFields = stringFields + ", qth";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "RST_SENT")
            {
                //qDebug() << "FileManager::processQsoReadingADIF-rst_sent: " << data << endl;
                stringFields = stringFields + ", rst_sent";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "RST_RCVD")
            {
                //qDebug() << "FileManager::processQsoReadingADIF-rst_rcvd: " << data << endl;
                stringFields = stringFields + ", rst_rcvd";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "RX_POWER")
            {
                stringFields = stringFields + ", rx_power";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "TX_PWR")
            {
                //qDebug() << "FileManager::processQsoReadingADIF-tx_pwr: " << data << endl;
                stringFields = stringFields + ", tx_pwr";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "SAT_MODE")
            {
                stringFields = stringFields + ", sat_mode";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "SAT_NAME")
            {
                stringFields = stringFields + ", sat_name";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "SFI")
            {
                stringFields = stringFields + ", sfi";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "SIG")
            {
                stringFields = stringFields + ", sig";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "SWL")
            {
                stringFields = stringFields + ", swl";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "TEN_TEN")
            {
                stringFields = stringFields + ", ten_ten";
                stringData = stringData + ", '" + data + "'";
            }
            else if (field == "WEB")
            {
                stringFields = stringFields + ", web";
                stringData = stringData + ", '" + data + "'";
            }
            else
            {
                QMessageBox msgBox;
                msgBox.setText("Unrecognized ADIF field: " + field);
                msgBox.setInformativeText("Do you want to ignore it?");
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::No);
                msgBox.exec();
            }
        }}
    }

    stringFields.remove(0,2);
    stringData.remove(0,2);
    queryString = "INSERT INTO log (" + stringFields + ") values (" + stringData + ")";

    //qDebug() << "FileManager::processQsoReadingADIF-stringFields2: " << stringFields  << endl;
    //qDebug() << "FileManager::processQsoReadingADIF-stringData2: " << stringData  << endl;
    qDebug() << "FileManager::processQsoReadingADIF-END: " << queryString  << endl;

     ret = query.exec(queryString);
     if(ret)
     {
         qDebug() << "FileManager::processQsoReadingADIF: TRUE" << endl;
     }else
     {
         qDebug() << "FileManager::processQsoReadingADIF: FALSE" << endl;
     }


    //printQs("processQsoReadingADIF", qs);
    return true;
}


bool FileManager::adifLogExport(const QString& _fileName)
{
    qDebug() << "FileManager::adifLogExport" << endl;


    return adifLogExportToFile(_fileName);

}


bool FileManager::adifLogExportToFile(const QString& _fileName){
qDebug() << "FileManager::adifLogExportToFile" << endl;


QFile file(_fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);

out << "ADIF v2.2.7 Export from Kontest\nhttp://jaime.robles.es/kontest\n<APP_KONTEST_NUMBER:5>0.0.1-alpha\n<PROGRAMID:7>KONTEST" << endl;
out << "<EOH>" << endl;


    QSqlQuery query("SELECT * FROM log");
    QSqlQuery query1;
    QSqlRecord rec = query.record();
    int nameCol;
    QString aux1, aux2, queryString;
/*
    QSqlQuery q("select * from employees");
    QSqlRecord rec = q.record();
    qDebug() << "Number of columns: " << rec.count();
    int nameCol = rec.indexOf("name"); // index of the field "name"
    while (q.next())
        qDebug() << q.value(nameCol).toString(); // output all names
*/

    while (query.next()) {

        nameCol = rec.indexOf("call");
        aux1 = (query.value(nameCol)).toString();
        //qDebug() << "FileManager::adifLogExportToFile: " << QString::number(nameCol) << "/" << aux1 << endl;
        out << "<CALL:" << QString::number(aux1.length()) << ">" << aux1 << " ";

        nameCol = rec.indexOf("qso_date");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length()) == 10){
            aux1.remove(QChar('-'), Qt::CaseInsensitive);
            aux1.remove(QChar('/'), Qt::CaseInsensitive);
            out << "<QSO_DATE:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("qso_date_off");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length()) == 10){
            aux1.remove(QChar('-'), Qt::CaseInsensitive);
            out << "<QSO_DATE_OFF:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("time_on");
        aux1 = (query.value(nameCol)).toString();
        if ( ((aux1.length()) == 5) || ((aux1.length()) == 7) ){
            aux1.remove(QChar(':'), Qt::CaseInsensitive);
            out << "<TIME_ON:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("time_off");
        aux1 = (query.value(nameCol)).toString();
        if ( ((aux1.length()) == 5) || ((aux1.length()) == 7) ){
            aux1.remove(QChar(':'), Qt::CaseInsensitive);
            out << "<TIME_OFF:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("bandid");
        aux1 = (query.value(nameCol)).toString();
        queryString = QString("SELECT name FROM band WHERE id='%1'").arg(aux1);
        query1.exec(queryString);
        query1.next();

        aux1 = (query1.value(0)).toString();
        out << "<BAND:" << QString::number(aux1.length()) << ">" << aux1  << " ";

        nameCol = rec.indexOf("band_rx");
        aux1 = (query.value(nameCol)).toString();

        if ( (0 < aux1.toInt() < 30) && (aux1.length()>0) ){
            queryString = QString("SELECT name FROM band WHERE id='%1'").arg(aux1);
            query1.exec(queryString);
            query1.next();

            aux1 = (query1.value(0)).toString();
            out << "<BAND_RX:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("modeid");
        aux1 = (query.value(nameCol)).toString();
        queryString = QString("SELECT name FROM mode WHERE id='%1'").arg(aux1);
        query1.exec(queryString);
        query1.next();

        aux1 = (query1.value(0)).toString();
        out << "<MODE:" << QString::number(aux1.length()) << ">" << aux1  << " ";

        nameCol = rec.indexOf("srx");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<SRX:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("srx_string");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<SRX_STRING:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("stx");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<STX:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("stx_string");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<STX_STRING:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("cqz");
        aux1 = (query.value(nameCol)).toString();
        if ( ((aux1.length())>0) && (0 < aux1.toInt() < 41) ){
            out << "<CQZ:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("ituz");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<ITUZ:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("dxcc");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<DXCC:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("address");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<ADDRESS:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("age");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<AGE:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("cnty");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<CNTY:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("comment");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<COMMENT:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("a_index");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<A_INDEX:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("ant_az");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<ANT_AZ:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("ant_el");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<ANT_EL:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("ant_path");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<ANT_PATH:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("arrl_sect");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<ARRL_SECT:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("checkcontest");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<CHECKCONTEST:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("class");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<CLASS:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("contacted_op");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<CONTACTED_OP:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("contest_id");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<CONTEST_ID:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("country");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<COUNTRY:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("credit_submitted");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<CREDIT_SUBMITTED:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("credit_granted");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<CREDIT_GRANTED:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("distance");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<DISTANCE:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("eq_call");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<EQ_CALL:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("email");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<EMAIL:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }


        nameCol = rec.indexOf("eqsl_qslrdate");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length()) == 10){
            aux1.remove(QChar('-'), Qt::CaseInsensitive);
            out << "<EQSL_QSLRDATE:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("eqsl_qslsdate");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length()) == 10){
            aux1.remove(QChar('-'), Qt::CaseInsensitive);
            out << "<EQSL_QSLSDATE:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("eqsl_qsl_rcvd");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())==1){
            out << "<EQSL_QSL_RCVD:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }
        nameCol = rec.indexOf("eqsl_qsl_sent");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())==1){
            out << "<EQSL_QSL_SENT:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("force_init");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<FORCE_INIT:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }
        nameCol = rec.indexOf("freq");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<FREQ:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }
        nameCol = rec.indexOf("freq_rx");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<FREQ_RX:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("gridsquare");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<GRIDSQUARE:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("my_gridsquare");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<MY_GRIDSQUARE:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("iota");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())==6){
            out << "<IOTA:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }
        nameCol = rec.indexOf("iota_island_id");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<IOTA_ISLAND_ID:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("my_iota");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())==6){
            out << "<MY_IOTA:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }
        nameCol = rec.indexOf("my_iota_island_id");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<MY_IOTA_ISLAND_ID:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }


        nameCol = rec.indexOf("k_index");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<K_INDEX:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("lat");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<LAT:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("lon");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<LON:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("my_lat");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<MY_LAT:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("my_lon");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<MY_LON:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("lotw_qslrdate");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length()) == 10){
            aux1.remove(QChar('-'), Qt::CaseInsensitive);
            out << "<LOTW_QSLRDATE:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("lotw_qslsdate");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length()) == 10){
            aux1.remove(QChar('-'), Qt::CaseInsensitive);
            out << "<LOTW_QSLSDATE:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("lotw_qsl_rcvd");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())==1){
            out << "<LOTW_QSL_RCVD:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }
        nameCol = rec.indexOf("lotw_qsl_sent");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())==1){
            out << "<LOTW_QSL_SENT:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("max_bursts");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<MAX_BURSTS:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("ms_shower");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<MS_SHOWER:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("my_city");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<MY_CITY:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("my_cnty");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<MY_CNTY:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("my_country");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<MY_COUNTRY:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("my_cq_zone");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<MY_CQ_ZONE:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("my_name");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<MY_NAME:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("name");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<NAME:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("my_operator");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<MY_OPERATOR:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("operator");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<OPERATOR:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("station_callsign");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<STATION_CALLSIGN:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }
        nameCol = rec.indexOf("owner_callsign");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<OWNER_CALLSIGN:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("my_rig");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<MY_RIG:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("my_sig");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<MY_SIG:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("my_sig_info");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<MY_SIG_INFO:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("my_state");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<MY_STATE:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("state");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<STATE:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("my_street");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<MY_STREET:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("notes");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<NOTES:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("nr_bursts");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<NR_BURSTS:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("nr_pings");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<NR_PINGS:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("pfx");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<PFX:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }
        nameCol = rec.indexOf("precedence");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<PRECEDENCE:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("prop_mode");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>1){
            out << "<PROP_MODE:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("public_key");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<PUBLIC_KEY:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("qslmsg");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<QSLMSG:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("qslrdate");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length()) == 10){
            aux1.remove(QChar('-'), Qt::CaseInsensitive);
            out << "<QSLRDATE:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("qslsdate");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length()) == 10){
            aux1.remove(QChar('-'), Qt::CaseInsensitive);
            out << "<QSLSDATE:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("qsl_rcvd");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())==1){
            out << "<QSL_RCVD:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }
        nameCol = rec.indexOf("qsl_sent");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())==1){
            out << "<QSL_SENT:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("qsl_rcvd_via");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())==1){
            out << "<QSL_RCVD_VIA:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }
        nameCol = rec.indexOf("qsl_sent_via");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())==1){
            out << "<QSL_SENT_VIA:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("qsl_via");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<QSL_VIA:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("qso_complete");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<QSO_COMPLETE:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("qso_random");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<QSO_RANDOM:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("qth");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<QTH:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("rst_sent");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<RST_SENT:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("rst_rcvd");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<RST_RCVD:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("rx_power");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<RX_POWER:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("tx_pwr");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<TX_PWR:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("sat_mode");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<SAT_MODE:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("sat_name");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<SAT_NAME:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("sfi");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<SFI:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("sig");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<SIG_INFO:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("swl");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<SWL:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("ten_ten");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<TEN_TEN:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        nameCol = rec.indexOf("web");
        aux1 = (query.value(nameCol)).toString();
        if ((aux1.length())>0){
            out << "<WEB:" << QString::number(aux1.length()) << ">" << aux1  << " ";
        }

        out << "<EOR> " << endl;
    }


}


bool FileManager::cabrilloLogExport(const QString& _fileName)
{
    qDebug() << "FileManager::cabrilloLogExport" << endl;


    return cabrilloLogExportToFile(_fileName);

}


bool FileManager::cabrilloLogExportToFile(const QString& _fileName){
qDebug() << "FileManager::cabrilloLogExportToFile" << endl;


    return false;
}



bool FileManager::printQs(const QString _q, const QStringList _line){
    QStringList qs = _line;

    qDebug() << "FileManager::printQs: begining: " << _q << endl;
    for (int i = 0; i<qs.size(); i++){
        qDebug() << qs.at(i) << " ";
    }
    qDebug() <<  endl;
    qDebug() << "FileManager::printQs: end :" << _q << endl;
    return true;
}
