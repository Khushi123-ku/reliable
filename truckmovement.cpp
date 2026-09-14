#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <cctype>
#include <cstdlib>

using namespace std;

// ============================================================
// RELIABLE LOGISTICS SOLUTIONS PVT. LTD.
// TRUCK MANAGEMENT SYSTEM
// Reliable + Prakash
// ============================================================

const string DATA_FILE = "truck_records.txt";
const string CSV_FILE = "truck_report.csv";
const string HTML_FILE = "truck_dashboard.html";

vector<struct TruckRecord> records;

// ============================================================
// DATA STRUCTURE
// ============================================================

struct TruckRecord
{
    int id;

    string truckNumber;
    string driverName;
    string mobile;
    string company;

    string entryDate;
    string entryTime;

    string exitDate;
    string exitTime;

    string status;
};

// ============================================================
// STRING FUNCTIONS
// ============================================================

string trim(const string& str)
{
    size_t first = str.find_first_not_of(" \t\r\n");

    if (first == string::npos)
        return "";

    size_t last = str.find_last_not_of(" \t\r\n");

    return str.substr(first, last - first + 1);
}

string upperCase(string text)
{
    transform(
        text.begin(),
        text.end(),
        text.begin(),
        [](unsigned char c)
        {
            return static_cast<char>(toupper(c));
        }
    );

    return text;
}

string htmlEscape(const string& text)
{
    string result;

    for (char c : text)
    {
        switch (c)
        {
            case '&':
                result += "&amp;";
                break;

            case '<':
                result += "&lt;";
                break;

            case '>':
                result += "&gt;";
                break;

            case '"':
                result += "&quot;";
                break;

            case '\'':
                result += "&#39;";
                break;

            default:
                result += c;
        }
    }

    return result;
}

string urlEncode(const string& value)
{
    ostringstream escaped;

    escaped.fill('0');
    escaped << hex;

    for (unsigned char c : value)
    {
        if (
            isalnum(c) ||
            c == '-' ||
            c == '_' ||
            c == '.' ||
            c == '~'
        )
        {
            escaped << c;
        }
        else if (c == '\n')
        {
            escaped << "%0A";
        }
        else
        {
            escaped
                << '%'
                << uppercase
                << setw(2)
                << int(c)
                << nouppercase;
        }
    }

    return escaped.str();
}

// ============================================================
// CURRENT DATE
// FORMAT: DD-MM-YYYY
// ============================================================

string getCurrentDate()
{
    time_t now = time(nullptr);

    tm* localTime = localtime(&now);

    stringstream ss;

    ss << setfill('0')
       << setw(2)
       << localTime->tm_mday
       << "-"
       << setw(2)
       << localTime->tm_mon + 1
       << "-"
       << setw(4)
       << localTime->tm_year + 1900;

    return ss.str();
}

// ============================================================
// CURRENT TIME
// FORMAT: HH:MM AM/PM
// ============================================================

string getCurrentTime()
{
    time_t now = time(nullptr);

    tm* localTime = localtime(&now);

    int hour = localTime->tm_hour;
    int minute = localTime->tm_min;

    string ampm;

    if (hour >= 12)
        ampm = "PM";
    else
        ampm = "AM";

    int displayHour = hour % 12;

    if (displayHour == 0)
        displayHour = 12;

    stringstream ss;

    ss << setfill('0')
       << setw(2)
       << displayHour
       << ":"
       << setw(2)
       << minute
       << " "
       << ampm;

    return ss.str();
}

// ============================================================
// DATE VALIDATION
// DD-MM-YYYY
// ============================================================

bool isValidDate(const string& date)
{
    if (date.length() != 10)
        return false;

    if (date[2] != '-' || date[5] != '-')
        return false;

    for (int i = 0; i < 10; i++)
    {
        if (i == 2 || i == 5)
            continue;

        if (!isdigit(
            static_cast<unsigned char>(date[i])))
        {
            return false;
        }
    }

    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(3, 2));
    int year = stoi(date.substr(6, 4));

    if (year < 2000 || year > 2100)
        return false;

    if (month < 1 || month > 12)
        return false;

    int daysInMonth[] =
    {
        0,
        31,
        28,
        31,
        30,
        31,
        30,
        31,
        31,
        30,
        31,
        30,
        31
    };

    bool leapYear =
        (year % 400 == 0) ||
        (year % 4 == 0 && year % 100 != 0);

    if (leapYear)
        daysInMonth[2] = 29;

    if (
        day < 1 ||
        day > daysInMonth[month]
    )
    {
        return false;
    }

    return true;
}

// ============================================================
// TIME VALIDATION
// FORMAT: HH:MM AM/PM
// ============================================================

bool isValidTime(string timeValue)
{
    timeValue = trim(timeValue);

    if (timeValue.length() != 8)
        return false;

    if (timeValue[2] != ':' ||
        timeValue[5] != ' ')
    {
        return false;
    }

    string ampm =
        upperCase(timeValue.substr(6, 2));

    if (
        ampm != "AM" &&
        ampm != "PM"
    )
    {
        return false;
    }

    for (int i = 0; i < 5; i++)
    {
        if (i == 2)
            continue;

        if (!isdigit(
            static_cast<unsigned char>(timeValue[i])))
        {
            return false;
        }
    }

    int hour =
        stoi(timeValue.substr(0, 2));

    int minute =
        stoi(timeValue.substr(3, 2));

    if (
        hour < 1 ||
        hour > 12
    )
    {
        return false;
    }

    if (
        minute < 0 ||
        minute > 59
    )
    {
        return false;
    }

    return true;
}

// ============================================================
// INPUT FUNCTIONS
// ============================================================

string inputString(const string& message)
{
    string value;

    cout << message;

    getline(cin, value);

    return trim(value);
}

int inputInt(const string& message)
{
    while (true)
    {
        string value =
            inputString(message);

        try
        {
            size_t position = 0;

            int number =
                stoi(value, &position);

            if (position == value.length())
                return number;
        }
        catch (...)
        {
        }

        cout <<
            "Invalid input. Please enter a number.\n";
    }
}

// ============================================================
// NEXT ID
// ============================================================

int getNextID()
{
    int maxID = 0;

    for (const auto& record : records)
    {
        if (record.id > maxID)
            maxID = record.id;
    }

    return maxID + 1;
}

// ============================================================
// FIND CURRENTLY INSIDE TRUCK
// ============================================================

int findInsideTruck(
    const string& truckNumber
)
{
    string target =
        upperCase(trim(truckNumber));

    for (size_t i = 0; i < records.size(); i++)
    {
        if (
            upperCase(records[i].truckNumber)
            == target
            &&
            upperCase(records[i].status)
            == "INSIDE"
        )
        {
            return static_cast<int>(i);
        }
    }

    return -1;
}

// ============================================================
// SAVE DATA PERMANENTLY
// ============================================================

void saveRecords()
{
    ofstream file(DATA_FILE);

    if (!file)
    {
        cout <<
            "\nERROR: Cannot save "
            << DATA_FILE
            << "\n";

        return;
    }

    for (const auto& r : records)
    {
        file
            << r.id << "|"
            << r.truckNumber << "|"
            << r.driverName << "|"
            << r.mobile << "|"
            << r.company << "|"
            << r.entryDate << "|"
            << r.entryTime << "|"
            << r.exitDate << "|"
            << r.exitTime << "|"
            << r.status
            << "\n";
    }

    file.close();
}

// ============================================================
// LOAD DATA
// ============================================================

void loadRecords()
{
    records.clear();

    ifstream file(DATA_FILE);

    if (!file)
        return;

    string line;

    while (getline(file, line))
    {
        if (trim(line).empty())
            continue;

        stringstream ss(line);

        TruckRecord r;

        string idText;

        try
        {
            getline(ss, idText, '|');

            r.id =
                stoi(idText);

            getline(ss, r.truckNumber, '|');
            getline(ss, r.driverName, '|');
            getline(ss, r.mobile, '|');
            getline(ss, r.company, '|');

            getline(ss, r.entryDate, '|');
            getline(ss, r.entryTime, '|');

            getline(ss, r.exitDate, '|');
            getline(ss, r.exitTime, '|');

            getline(ss, r.status, '|');

            records.push_back(r);
        }
        catch (...)
        {
            // Ignore invalid record
        }
    }

    file.close();
}

// ============================================================
// CSV ESCAPE
// ============================================================

string csvEscape(const string& value)
{
    string result = "\"";

    for (char c : value)
    {
        if (c == '"')
            result += "\"\"";
        else
            result += c;
    }

    result += "\"";

    return result;
}

// ============================================================
// GENERATE CSV
// ============================================================

void generateCSV()
{
    ofstream file(CSV_FILE);

    if (!file)
    {
        cout <<
            "ERROR: Cannot create CSV report.\n";

        return;
    }

    file
        << "ID,"
        << "Truck Number,"
        << "Driver Name,"
        << "Mobile,"
        << "Company,"
        << "Entry Date,"
        << "Entry Time,"
        << "Exit Date,"
        << "Exit Time,"
        << "Status\n";

    for (const auto& r : records)
    {
        file
            << r.id << ","
            << csvEscape(r.truckNumber) << ","
            << csvEscape(r.driverName) << ","
            << csvEscape(r.mobile) << ","
            << csvEscape(r.company) << ","
            << csvEscape(r.entryDate) << ","
            << csvEscape(r.entryTime) << ","
            << csvEscape(r.exitDate) << ","
            << csvEscape(r.exitTime) << ","
            << csvEscape(r.status)
            << "\n";
    }

    file.close();
}

// ============================================================
// STATISTICS
// ============================================================

int getInsideCount()
{
    int count = 0;

    for (const auto& r : records)
    {
        if (
            upperCase(r.status)
            == "INSIDE"
        )
        {
            count++;
        }
    }

    return count;
}

int getExitedCount()
{
    int count = 0;

    for (const auto& r : records)
    {
        if (
            upperCase(r.status)
            == "EXITED"
        )
        {
            count++;
        }
    }

    return count;
}

int getReliableCount()
{
    int count = 0;

    for (const auto& r : records)
    {
        if (
            upperCase(r.company)
            == "RELIABLE"
        )
        {
            count++;
        }
    }

    return count;
}

int getPrakashCount()
{
    int count = 0;

    for (const auto& r : records)
    {
        if (
            upperCase(r.company)
            == "PRAKASH"
        )
        {
            count++;
        }
    }

    return count;
}

// ============================================================
// GENERATE PREMIUM HTML DASHBOARD
// ============================================================

void generateHTML()
{
    ofstream file(HTML_FILE);

    if (!file)
    {
        cout <<
            "ERROR: Cannot create HTML dashboard.\n";

        return;
    }

    int total =
        static_cast<int>(records.size());

    int inside =
        getInsideCount();

    int exited =
        getExitedCount();

    int reliable =
        getReliableCount();

    int prakash =
        getPrakashCount();

    file << R"HTML(
<!DOCTYPE html>
<html lang="en">

<head>

<meta charset="UTF-8">

<meta name="viewport"
content="width=device-width, initial-scale=1.0">

<title>
Reliable Logistics - Truck Management
</title>

<style>

* {
    box-sizing: border-box;
    margin: 0;
    padding: 0;
}

body {
    font-family:
        "Segoe UI",
        Arial,
        sans-serif;

    background: #f5f7fa;
    color: #20252b;
}

.top-line {
    height: 6px;
    background: #c71920;
}

.header {
    background: white;
    border-bottom: 1px solid #e5e7eb;
    padding: 22px 5%;
    box-shadow:
        0 4px 18px rgba(0,0,0,.06);
}

.header-inner {
    max-width: 1500px;
    margin: auto;

    display: flex;

    justify-content: space-between;

    align-items: center;

    gap: 25px;
}

.logo {
    width: 310px;
    max-width: 100%;
    height: auto;
    display: block;
}

.header-right {
    text-align: right;
}

.header-title {
    font-size: 25px;
    font-weight: 800;
    color: #252a30;
}

.header-subtitle {
    color: #777;
    font-size: 13px;
    margin-top: 5px;
}

.button-row {
    display: flex;
    gap: 10px;
    flex-wrap: wrap;
    margin-top: 14px;
    justify-content: flex-end;
}

.btn {
    border: none;
    border-radius: 8px;
    padding: 11px 17px;
    font-weight: 700;
    cursor: pointer;
    font-size: 13px;
    transition: .2s;
}

.btn:hover {
    transform: translateY(-2px);
}

.btn-print {
    background: #252a30;
    color: white;
}

.btn-whatsapp {
    background: #25D366;
    color: white;
}

.container {
    width: 90%;
    max-width: 1500px;
    margin: 30px auto;
}

.welcome {
    background:
        linear-gradient(
            135deg,
            #ffffff,
            #fff8f8
        );

    border-left: 5px solid #c71920;

    border-radius: 12px;

    padding: 20px 22px;

    margin-bottom: 22px;

    box-shadow:
        0 5px 18px rgba(0,0,0,.05);
}

.welcome h2 {
    font-size: 21px;
    color: #c71920;
}

.welcome p {
    margin-top: 5px;
    color: #666;
    font-size: 13px;
}

.cards {
    display: grid;
    grid-template-columns:
        repeat(5, 1fr);

    gap: 16px;

    margin-bottom: 23px;
}

.card {
    background: white;

    border: 1px solid #e5e7eb;

    border-radius: 13px;

    padding: 20px;

    box-shadow:
        0 6px 22px rgba(0,0,0,.05);

    position: relative;

    overflow: hidden;
}

.card::before {
    content: "";

    position: absolute;

    top: 0;
    left: 0;
    right: 0;

    height: 4px;

    background: #c71920;
}

.card-title {
    color: #73787e;
    font-size: 12px;
    font-weight: 700;
    text-transform: uppercase;
    letter-spacing: .5px;
}

.card-number {
    font-size: 32px;
    font-weight: 800;
    margin-top: 8px;
    color: #252a30;
}

.card-description {
    font-size: 11px;
    color: #9aa0a6;
    margin-top: 4px;
}

.filter-box {
    background: white;

    border: 1px solid #e5e7eb;

    border-radius: 13px;

    padding: 21px;

    margin-bottom: 22px;

    box-shadow:
        0 6px 22px rgba(0,0,0,.05);
}

.filter-heading {
    font-size: 18px;
    font-weight: 800;
    margin-bottom: 15px;
}

.filters {
    display: grid;

    grid-template-columns:
        repeat(4, 1fr);

    gap: 12px;
}

input,
select {
    width: 100%;

    padding: 12px 13px;

    border: 1px solid #d9dde2;

    border-radius: 8px;

    background: white;

    outline: none;

    font-size: 13px;
}

input:focus,
select:focus {
    border-color: #c71920;

    box-shadow:
        0 0 0 3px
        rgba(199,25,32,.08);
}

.table-box {
    background: white;

    border: 1px solid #e5e7eb;

    border-radius: 13px;

    overflow: hidden;

    box-shadow:
        0 6px 22px rgba(0,0,0,.05);
}

.table-header {
    padding: 18px 21px;

    border-bottom:
        1px solid #e5e7eb;

    display: flex;

    justify-content:
        space-between;

    align-items: center;
}

.table-header h2 {
    font-size: 18px;
}

.showing {
    font-size: 12px;
    color: #777;
}

.table-scroll {
    overflow-x: auto;
}

table {
    width: 100%;
    min-width: 1250px;
    border-collapse: collapse;
}

th {
    background: #f7f8f9;

    color: #565d64;

    font-size: 11px;

    text-transform: uppercase;

    letter-spacing: .5px;

    text-align: left;

    padding: 14px 15px;

    white-space: nowrap;
}

td {
    padding: 14px 15px;

    border-top:
        1px solid #edf0f2;

    font-size: 13px;

    white-space: nowrap;
}

tr:hover td {
    background: #fff9f9;
}

.truck-column {
    min-width: 145px;
}

.driver-column {
    min-width: 200px;
}

.mobile-column {
    min-width: 155px;
}

.company-reliable {
    color: #c71920;
    font-weight: 800;
}

.company-prakash {
    color: #1769aa;
    font-weight: 800;
}

.badge {
    display: inline-block;

    padding: 6px 12px;

    border-radius: 20px;

    font-size: 10px;

    font-weight: 800;

    letter-spacing: .5px;
}

.inside {
    background: #e7f7ed;

    color: #16803d;

    border:
        1px solid #b9e8c8;
}

.exited {
    background: #f0f1f3;

    color: #555c63;

    border:
        1px solid #d8dce0;
}

.no-results {
    text-align: center;
    padding: 40px;
    color: #888;
}

.footer {
    margin-top: 30px;

    padding: 25px;

    text-align: center;

    font-size: 11px;

    color: #8a9096;

    border-top:
        1px solid #e5e7eb;
}

@media(max-width:1100px) {

    .cards {
        grid-template-columns:
            repeat(3, 1fr);
    }

    .filters {
        grid-template-columns:
            repeat(2, 1fr);
    }

    .header-inner {
        flex-direction: column;
        align-items: flex-start;
    }

    .header-right {
        text-align: left;
    }

    .button-row {
        justify-content: flex-start;
    }
}

@media(max-width:650px) {

    .container {
        width: 94%;
    }

    .cards {
        grid-template-columns:
            repeat(2, 1fr);
    }

    .filters {
        grid-template-columns: 1fr;
    }

    .header {
        padding: 18px 4%;
    }

    .logo {
        width: 270px;
    }

    .header-title {
        font-size: 20px;
    }
}

@media(max-width:430px) {

    .cards {
        grid-template-columns: 1fr;
    }

    .card-number {
        font-size: 28px;
    }
}

@media print {

    .button-row,
    .filter-box {
        display: none !important;
    }

    body {
        background: white;
    }

    .header {
        box-shadow: none;
    }

    .table-box {
        box-shadow: none;
    }
}

</style>

</head>

<body>

<div class="top-line"></div>

<header class="header">

<div class="header-inner">

<div>

<img
src="reliable_logo.png"
class="logo"
alt="Reliable Logistics Solutions"
>

</div>

<div class="header-right">

<div class="header-title">
Truck Management System
</div>

<div class="header-subtitle">
Vehicle Entry & Exit Monitoring
</div>

<div class="button-row">

<button
class="btn btn-print"
onclick="window.print()">
Print Report
</button>

<button
class="btn btn-whatsapp"
onclick="shareWhatsApp()">
Share on WhatsApp
</button>

</div>

</div>

</div>

</header>

<main class="container">

<section class="welcome">

<h2>
Reliable Logistics Solutions Pvt. Ltd.
</h2>

<p>
Professional Truck Entry, Exit & Driver Record Management
</p>

</section>

<section class="cards">

<div class="card">

<div class="card-title">
Total Records
</div>

<div class="card-number">
)HTML";

    file << total;

    file << R"HTML(
</div>

<div class="card-description">
All truck movements
</div>

</div>

<div class="card">

<div class="card-title">
Currently Inside
</div>

<div class="card-number">
)HTML";

    file << inside;

    file << R"HTML(
</div>

<div class="card-description">
Trucks currently inside
</div>

</div>

<div class="card">

<div class="card-title">
Exited
</div>

<div class="card-number">
)HTML";

    file << exited;

    file << R"HTML(
</div>

<div class="card-description">
Completed movements
</div>

</div>

<div class="card">

<div class="card-title">
Reliable
</div>

<div class="card-number">
)HTML";

    file << reliable;

    file << R"HTML(
</div>

<div class="card-description">
Reliable company
</div>

</div>

<div class="card">

<div class="card-title">
Prakash
</div>

<div class="card-number">
)HTML";

    file << prakash;

    file << R"HTML(
</div>

<div class="card-description">
Prakash company
</div>

</div>

</section>

<section class="filter-box">

<div class="filter-heading">
Search & Filter Records
</div>

<div class="filters">

<input
type="text"
id="truckFilter"
placeholder="Truck Number..."
>

<input
type="text"
id="dateFilter"
placeholder="Date DD-MM-YYYY..."
>

<select id="companyFilter">

<option value="">
All Companies
</option>

<option value="Reliable">
Reliable
</option>

<option value="Prakash">
Prakash
</option>

</select>

<select id="statusFilter">

<option value="">
All Status
</option>

<option value="INSIDE">
INSIDE
</option>

<option value="EXITED">
EXITED
</option>

</select>

</div>

</section>

<section class="table-box">

<div class="table-header">

<h2>
Truck Movement Records
</h2>

<div class="showing">

Showing
<strong id="visibleCount">0</strong>
records

</div>

</div>

<div class="table-scroll">

<table>

<thead>

<tr>

<th>ID</th>

<th class="truck-column">
Truck Number
</th>

<th class="driver-column">
Driver Name
</th>

<th class="mobile-column">
Mobile
</th>

<th>
Company
</th>

<th>
Entry Date
</th>

<th>
Entry Time
</th>

<th>
Exit Date
</th>

<th>
Exit Time
</th>

<th>
Status
</th>

</tr>

</thead>

<tbody id="tableBody">
)HTML";

    // ========================================================
    // TABLE RECORDS
    // ========================================================

    for (const auto& r : records)
    {
        string companyClass =
            upperCase(r.company) == "RELIABLE"
            ? "company-reliable"
            : "company-prakash";

        string badgeClass =
            upperCase(r.status) == "INSIDE"
            ? "inside"
            : "exited";

        file << R"HTML(

<tr
class="data-row"

data-truck=")HTML"
             << htmlEscape(r.truckNumber)
             << R"HTML("

data-entry-date=")HTML"
             << htmlEscape(r.entryDate)
             << R"HTML("

data-exit-date=")HTML"
             << htmlEscape(r.exitDate)
             << R"HTML("

data-company=")HTML"
             << htmlEscape(r.company)
             << R"HTML("

data-status=")HTML"
             << htmlEscape(r.status)
             << R"HTML("
>

<td>
)HTML"
             << r.id
             << R"HTML(
</td>

<td>
<strong>
)HTML"
             << htmlEscape(r.truckNumber)
             << R"HTML(
</strong>
</td>

<td>
)HTML"
             << htmlEscape(r.driverName)
             << R"HTML(
</td>

<td>
)HTML"
             << htmlEscape(r.mobile)
             << R"HTML(
</td>

<td>
<span class=")HTML"
             << companyClass
             << R"HTML(">
)HTML"
             << htmlEscape(r.company)
             << R"HTML(
</span>
</td>

<td>
)HTML"
             << htmlEscape(r.entryDate)
             << R"HTML(
</td>

<td>
)HTML"
             << htmlEscape(r.entryTime)
             << R"HTML(
</td>

<td>
)HTML"
             << (
                 r.exitDate.empty()
                 ? "-"
                 : htmlEscape(r.exitDate)
             )
             << R"HTML(
</td>

<td>
)HTML"
             << (
                 r.exitTime.empty()
                 ? "-"
                 : htmlEscape(r.exitTime)
             )
             << R"HTML(
</td>

<td>

<span class="badge )HTML"
             << badgeClass
             << R"HTML(">
)HTML"
             << htmlEscape(r.status)
             << R"HTML(
</span>

</td>

</tr>

)HTML";
    }

    file << R"HTML(

<tr id="noResults" style="display:none;">

<td colspan="10" class="no-results">

No matching records found.

</td>

</tr>

</tbody>

</table>

</div>

</section>

</main>

<footer class="footer">

Reliable Logistics Solutions Pvt. Ltd.
&nbsp; | &nbsp;
Truck Management System
&nbsp; | &nbsp;
Reliable + Prakash

</footer>

<script>

// ==========================================================
// FILTER
// ==========================================================

function filterRecords()
{
    const truck =
        document
        .getElementById("truckFilter")
        .value
        .toLowerCase()
        .trim();

    const date =
        document
        .getElementById("dateFilter")
        .value
        .toLowerCase()
        .trim();

    const company =
        document
        .getElementById("companyFilter")
        .value
        .toLowerCase()
        .trim();

    const status =
        document
        .getElementById("statusFilter")
        .value
        .toLowerCase()
        .trim();

    const rows =
        document.querySelectorAll(".data-row");

    let visible = 0;

    rows.forEach(function(row)
    {
        const rowTruck =
            row.dataset.truck.toLowerCase();

        const entryDate =
            row.dataset.entryDate.toLowerCase();

        const exitDate =
            row.dataset.exitDate.toLowerCase();

        const rowCompany =
            row.dataset.company.toLowerCase();

        const rowStatus =
            row.dataset.status.toLowerCase();

        const truckMatch =
            truck === "" ||
            rowTruck.includes(truck);

        const dateMatch =
            date === "" ||
            entryDate === date ||
            exitDate === date;

        const companyMatch =
            company === "" ||
            rowCompany === company;

        const statusMatch =
            status === "" ||
            rowStatus === status;

        const show =
            truckMatch &&
            dateMatch &&
            companyMatch &&
            statusMatch;

        if (show)
        {
            row.style.display = "";
            visible++;
        }
        else
        {
            row.style.display = "none";
        }
    });

    document
        .getElementById("visibleCount")
        .textContent = visible;

    document
        .getElementById("noResults")
        .style.display =
            visible === 0 ? "" : "none";
}

// ==========================================================
// FILTER EVENTS
// ==========================================================

document
    .getElementById("truckFilter")
    .addEventListener(
        "input",
        filterRecords
    );

document
    .getElementById("dateFilter")
    .addEventListener(
        "input",
        filterRecords
    );

document
    .getElementById("companyFilter")
    .addEventListener(
        "change",
        filterRecords
    );

document
    .getElementById("statusFilter")
    .addEventListener(
        "change",
        filterRecords
    );

// ==========================================================
// WHATSAPP
// ==========================================================

function shareWhatsApp()
{
    const message =
        "Reliable Logistics Solutions Pvt. Ltd.\\n" +
        "Truck Management Report\\n" +
        "--------------------------------\\n" +
        "Total Records: )HTML";

    file << total;

    file << R"HTML(\\n" +
        "Currently Inside: )HTML";

    file << inside;

    file << R"HTML(\\n" +
        "Exited: )HTML";

    file << exited;

    file << R"HTML(\\n" +
        "Reliable: )HTML";

    file << reliable;

    file << R"HTML(\\n" +
        "Prakash: )HTML";

    file << prakash;

    file << R"HTML(\\n" +
        "--------------------------------\\n" +
        "Truck Management System";

    const url =
        "https://wa.me/?text=" +
        encodeURIComponent(message);

    window.open(
        url,
        "_blank"
    );
}

// ==========================================================
// INITIAL FILTER
// ==========================================================

filterRecords();

</script>

</body>

</html>
)HTML";

    file.close();
}

// ============================================================
// TRUCK ENTRY
// ============================================================

void truckEntry()
{
    cout << "\n";
    cout << "====================================================\n";
    cout << "                    TRUCK ENTRY\n";
    cout << "====================================================\n";

    // --------------------------------------------------------
    // TRUCK NUMBER
    // --------------------------------------------------------

    string truckNumber;

    while (true)
    {
        truckNumber =
            upperCase(
                inputString(
                    "Enter Truck Number: "
                )
            );

        if (truckNumber.empty())
        {
            cout <<
                "Truck number cannot be empty.\n";

            continue;
        }

        if (
            findInsideTruck(truckNumber)
            != -1
        )
        {
            cout << "\n";
            cout <<
                "This truck is already INSIDE.\n";

            cout <<
                "Please exit it before creating another entry.\n";

            return;
        }

        break;
    }

    // --------------------------------------------------------
    // DRIVER NAME
    // --------------------------------------------------------

    string driverName;

    while (true)
    {
        driverName =
            inputString(
                "Enter Driver Name: "
            );

        if (!driverName.empty())
            break;

        cout <<
            "Driver name cannot be empty.\n";
    }

    // --------------------------------------------------------
    // MOBILE
    // --------------------------------------------------------

    string mobile;

    while (true)
    {
        mobile =
            inputString(
                "Enter Driver Mobile: "
            );

        if (!mobile.empty())
            break;

        cout <<
            "Mobile number cannot be empty.\n";
    }

    // --------------------------------------------------------
    // COMPANY
    // --------------------------------------------------------

    string company;

    while (true)
    {
        cout << "\n";

        cout << "Select Company:\n";
        cout << "1. Reliable\n";
        cout << "2. Prakash\n";

        int choice =
            inputInt(
                "Enter choice: "
            );

        if (choice == 1)
        {
            company = "Reliable";
            break;
        }

        if (choice == 2)
        {
            company = "Prakash";
            break;
        }

        cout <<
            "Invalid company choice.\n";
    }

    // --------------------------------------------------------
    // ENTRY DATE
    // --------------------------------------------------------

    string entryDate;

    while (true)
    {
        cout << "\n";

        cout << "Entry Date:\n";
        cout << "1. Automatic current date\n";
        cout << "2. Manual date\n";

        int dateMode =
            inputInt(
                "Enter choice: "
            );

        if (dateMode == 1)
        {
            entryDate =
                getCurrentDate();

            cout <<
                "Automatic Date: "
                << entryDate
                << "\n";

            break;
        }

        if (dateMode == 2)
        {
            while (true)
            {
                entryDate =
                    inputString(
                        "Enter Date (DD-MM-YYYY): "
                    );

                if (isValidDate(entryDate))
                    break;

                cout <<
                    "Invalid date.\n";

                cout <<
                    "Example: 03-09-2026\n";
            }

            break;
        }

        cout <<
            "Invalid choice. Please select 1 or 2.\n";
    }

    // --------------------------------------------------------
    // ENTRY TIME
    // --------------------------------------------------------

    string entryTime;

    while (true)
    {
        cout << "\n";

        cout << "Entry Time:\n";
        cout << "1. Automatic current time\n";
        cout << "2. Manual time\n";

        int timeMode =
            inputInt(
                "Enter choice: "
            );

        if (timeMode == 1)
        {
            entryTime =
                getCurrentTime();

            cout <<
                "Automatic Time: "
                << entryTime
                << "\n";

            break;
        }

        if (timeMode == 2)
        {
            while (true)
            {
                entryTime =
                    inputString(
                        "Enter Time (HH:MM AM/PM): "
                    );

                entryTime =
                    upperCase(entryTime);

                if (isValidTime(entryTime))
                    break;

                cout <<
                    "Invalid time.\n";

                cout <<
                    "Examples: 09:30 AM or 02:45 PM\n";
            }

            break;
        }

        cout <<
            "Invalid choice. Please select 1 or 2.\n";
    }

    // --------------------------------------------------------
    // CREATE RECORD
    // --------------------------------------------------------

    TruckRecord r;

    r.id =
        getNextID();

    r.truckNumber =
        truckNumber;

    r.driverName =
        driverName;

    r.mobile =
        mobile;

    r.company =
        company;

    r.entryDate =
        entryDate;

    r.entryTime =
        entryTime;

    r.exitDate =
        "";

    r.exitTime =
        "";

    r.status =
        "INSIDE";

    records.push_back(r);

    // --------------------------------------------------------
    // SAVE
    // --------------------------------------------------------

    saveRecords();
    generateCSV();
    generateHTML();

    cout << "\n";
    cout << "====================================================\n";
    cout << "              TRUCK ENTRY SUCCESSFUL\n";
    cout << "====================================================\n";

    cout <<
        "ID          : "
        << r.id
        << "\n";

    cout <<
        "Truck       : "
        << r.truckNumber
        << "\n";

    cout <<
        "Driver      : "
        << r.driverName
        << "\n";

    cout <<
        "Mobile      : "
        << r.mobile
        << "\n";

    cout <<
        "Company     : "
        << r.company
        << "\n";

    cout <<
        "Entry Date  : "
        << r.entryDate
        << "\n";

    cout <<
        "Entry Time  : "
        << r.entryTime
        << "\n";

    cout <<
        "Status      : INSIDE\n";

    cout << "====================================================\n";
}

// ============================================================
// TRUCK EXIT
// ============================================================

void truckExit()
{
    cout << "\n";
    cout << "====================================================\n";
    cout << "                     TRUCK EXIT\n";
    cout << "====================================================\n";

    string truckNumber =
        upperCase(
            inputString(
                "Enter Truck Number: "
            )
        );

    int index =
        findInsideTruck(
            truckNumber
        );

    if (index == -1)
    {
        cout << "\n";

        cout <<
            "No INSIDE truck found with number: "
            << truckNumber
            << "\n";

        return;
    }

    TruckRecord& r =
        records[index];

    cout << "\n";
    cout << "Truck Found\n";
    cout << "---------------------------------------------\n";

    cout <<
        "Truck   : "
        << r.truckNumber
        << "\n";

    cout <<
        "Driver  : "
        << r.driverName
        << "\n";

    cout <<
        "Mobile  : "
        << r.mobile
        << "\n";

    cout <<
        "Company : "
        << r.company
        << "\n";

    cout <<
        "Entry   : "
        << r.entryDate
        << " "
        << r.entryTime
        << "\n";

    cout << "---------------------------------------------\n";

    // --------------------------------------------------------
    // EXIT DATE
    // --------------------------------------------------------

    while (true)
    {
        cout << "\n";

        cout << "Exit Date:\n";
        cout << "1. Automatic current date\n";
        cout << "2. Manual date\n";

        int dateMode =
            inputInt(
                "Enter choice: "
            );

        if (dateMode == 1)
        {
            r.exitDate =
                getCurrentDate();

            break;
        }

        if (dateMode == 2)
        {
            while (true)
            {
                r.exitDate =
                    inputString(
                        "Enter Exit Date (DD-MM-YYYY): "
                    );

                if (isValidDate(r.exitDate))
                    break;

                cout <<
                    "Invalid date.\n";
            }

            break;
        }

        cout <<
            "Invalid choice. Please select 1 or 2.\n";
    }

    // --------------------------------------------------------
    // EXIT TIME
    // --------------------------------------------------------

    while (true)
    {
        cout << "\n";

        cout << "Exit Time:\n";
        cout << "1. Automatic current time\n";
        cout << "2. Manual time\n";

        int timeMode =
            inputInt(
                "Enter choice: "
            );

        if (timeMode == 1)
        {
            r.exitTime =
                getCurrentTime();

            break;
        }

        if (timeMode == 2)
        {
            while (true)
            {
                r.exitTime =
                    inputString(
                        "Enter Exit Time (HH:MM AM/PM): "
                    );

                r.exitTime =
                    upperCase(r.exitTime);

                if (isValidTime(r.exitTime))
                    break;

                cout <<
                    "Invalid time.\n";

                cout <<
                    "Examples: 09:30 AM or 05:45 PM\n";
            }

            break;
        }

        cout <<
            "Invalid choice. Please select 1 or 2.\n";
    }

    r.status =
        "EXITED";

    // --------------------------------------------------------
    // SAVE
    // --------------------------------------------------------

    saveRecords();
    generateCSV();
    generateHTML();

    cout << "\n";
    cout << "====================================================\n";
    cout << "               TRUCK EXIT SUCCESSFUL\n";
    cout << "====================================================\n";

    cout <<
        "Truck      : "
        << r.truckNumber
        << "\n";

    cout <<
        "Driver     : "
        << r.driverName
        << "\n";

    cout <<
        "Company    : "
        << r.company
        << "\n";

    cout <<
        "Exit Date  : "
        << r.exitDate
        << "\n";

    cout <<
        "Exit Time  : "
        << r.exitTime
        << "\n";

    cout <<
        "Status     : EXITED\n";

    cout << "====================================================\n";
}

// ============================================================
// SHOW RECORDS
// ============================================================

void showRecords(
    const vector<TruckRecord>& list
)
{
    if (list.empty())
    {
        cout <<
            "\nNo records found.\n";

        return;
    }

    cout << "\n";

    cout <<
        "====================================================================================================\n";

    cout
        << left
        << setw(5) << "ID"
        << setw(16) << "TRUCK"
        << setw(21) << "DRIVER"
        << setw(16) << "MOBILE"
        << setw(12) << "COMPANY"
        << setw(13) << "ENTRY DATE"
        << setw(13) << "ENTRY TIME"
        << setw(13) << "EXIT DATE"
        << setw(13) << "EXIT TIME"
        << setw(10) << "STATUS"
        << "\n";

    cout <<
        "====================================================================================================\n";

    for (const auto& r : list)
    {
        cout
            << left
            << setw(5)
            << r.id

            << setw(16)
            << r.truckNumber

            << setw(21)
            << r.driverName

            << setw(16)
            << r.mobile

            << setw(12)
            << r.company

            << setw(13)
            << r.entryDate

            << setw(13)
            << r.entryTime

            << setw(13)
            << (
                r.exitDate.empty()
                ? "-"
                : r.exitDate
            )

            << setw(13)
            << (
                r.exitTime.empty()
                ? "-"
                : r.exitTime
            )

            << setw(10)
            << r.status

            << "\n";
    }

    cout <<
        "====================================================================================================\n";
}

// ============================================================
// SEARCH BY DATE
// ============================================================

void searchByDate()
{
    cout << "\n";
    cout << "====================================================\n";
    cout << "                 SEARCH BY DATE\n";
    cout << "====================================================\n";

    string date;

    while (true)
    {
        date =
            inputString(
                "Enter Date (DD-MM-YYYY): "
            );

        if (isValidDate(date))
            break;

        cout <<
            "Invalid date.\n";
    }

    vector<TruckRecord> result;

    for (const auto& r : records)
    {
        if (
            r.entryDate == date ||
            r.exitDate == date
        )
        {
            result.push_back(r);
        }
    }

    cout <<
        "\nRecords for: "
        << date
        << "\n";

    showRecords(result);
}

// ============================================================
// SEARCH BY TRUCK
// ============================================================

void searchByTruck()
{
    cout << "\n";
    cout << "====================================================\n";
    cout << "                SEARCH BY TRUCK\n";
    cout << "====================================================\n";

    string truck =
        upperCase(
            inputString(
                "Enter Truck Number: "
            )
        );

    vector<TruckRecord> result;

    for (const auto& r : records)
    {
        if (
            upperCase(r.truckNumber)
            ==
            truck
        )
        {
            result.push_back(r);
        }
    }

    showRecords(result);
}

// ============================================================
// COMPANY-WISE RECORDS
// ============================================================

void searchByCompany()
{
    cout << "\n";
    cout << "====================================================\n";
    cout << "              COMPANY-WISE RECORDS\n";
    cout << "====================================================\n";

    cout << "1. Reliable\n";
    cout << "2. Prakash\n";

    int choice =
        inputInt(
            "Enter choice: "
        );

    string company;

    if (choice == 1)
        company = "RELIABLE";

    else if (choice == 2)
        company = "PRAKASH";

    else
    {
        cout <<
            "Invalid choice.\n";

        return;
    }

    vector<TruckRecord> result;

    for (const auto& r : records)
    {
        if (
            upperCase(r.company)
            ==
            company
        )
        {
            result.push_back(r);
        }
    }

    cout <<
        "\nCompany: "
        << company
        << "\n";

    showRecords(result);
}

// ============================================================
// CURRENTLY INSIDE
// ============================================================

void showInsideTrucks()
{
    cout << "\n";
    cout << "====================================================\n";
    cout << "             CURRENTLY INSIDE TRUCKS\n";
    cout << "====================================================\n";

    vector<TruckRecord> result;

    for (const auto& r : records)
    {
        if (
            upperCase(r.status)
            ==
            "INSIDE"
        )
        {
            result.push_back(r);
        }
    }

    showRecords(result);
}

// ============================================================
// SUMMARY
// ============================================================

void showSummary()
{
    cout << "\n";
    cout << "====================================================\n";
    cout << "                 DASHBOARD SUMMARY\n";
    cout << "====================================================\n";

    cout
        << left
        << setw(25)
        << "Total Records"
        << ": "
        << records.size()
        << "\n";

    cout
        << left
        << setw(25)
        << "Currently Inside"
        << ": "
        << getInsideCount()
        << "\n";

    cout
        << left
        << setw(25)
        << "Exited"
        << ": "
        << getExitedCount()
        << "\n";

    cout
        << left
        << setw(25)
        << "Reliable Records"
        << ": "
        << getReliableCount()
        << "\n";

    cout
        << left
        << setw(25)
        << "Prakash Records"
        << ": "
        << getPrakashCount()
        << "\n";

    cout << "====================================================\n";
}

// ============================================================
// WHATSAPP SHARE
// ============================================================

void shareWhatsApp()
{
    string message =
        "Reliable Logistics Solutions Pvt. Ltd.\n"
        "Truck Management Report\n"
        "--------------------------------\n"
        "Total Records: "
        + to_string(records.size())
        + "\n"
        "Currently Inside: "
        + to_string(getInsideCount())
        + "\n"
        "Exited: "
        + to_string(getExitedCount())
        + "\n"
        "Reliable: "
        + to_string(getReliableCount())
        + "\n"
        "Prakash: "
        + to_string(getPrakashCount())
        + "\n"
        "--------------------------------\n"
        "Truck Management System";

    string encoded =
        urlEncode(message);

    string whatsappURL =
        "https://wa.me/?text=" +
        encoded;

#ifdef _WIN32

    string command =
        "start \"\" \"" +
        whatsappURL +
        "\"";

    system(command.c_str());

#elif __APPLE__

    string command =
        "open \"" +
        whatsappURL +
        "\"";

    system(command.c_str());

#else

    string command =
        "xdg-open \"" +
        whatsappURL +
        "\"";

    system(command.c_str());

#endif

    cout <<
        "\nWhatsApp share page opened.\n";
}

// ============================================================
// OPEN DASHBOARD
// ============================================================

void openDashboard()
{
    generateHTML();

#ifdef _WIN32

    string command =
        "start \"\" \"" +
        HTML_FILE +
        "\"";

    system(command.c_str());

#elif __APPLE__

    string command =
        "open \"" +
        HTML_FILE +
        "\"";

    system(command.c_str());

#else

    string command =
        "xdg-open \"" +
        HTML_FILE +
        "\"";

    system(command.c_str());

#endif

    cout <<
        "\nPremium dashboard opened in browser.\n";
}

// ============================================================
// REGENERATE REPORTS
// ============================================================

void regenerateReports()
{
    saveRecords();

    generateCSV();

    generateHTML();

    cout << "\n";
    cout << "====================================================\n";
    cout << "                 REPORTS UPDATED\n";
    cout << "====================================================\n";

    cout <<
        "Permanent Data : "
        << DATA_FILE
        << "\n";

    cout <<
        "Excel CSV      : "
        << CSV_FILE
        << "\n";

    cout <<
        "HTML Dashboard : "
        << HTML_FILE
        << "\n";

    cout << "====================================================\n";
}

// ============================================================
// MENU
// ============================================================

void showMenu()
{
    cout << "\n\n";

    cout << "====================================================\n";
    cout << "      RELIABLE LOGISTICS SOLUTIONS PVT. LTD.\n";
    cout << "          CAMPUS TRUCK MANAGEMENT SYSTEM\n";
    cout << "                RELIABLE + PRAKASH\n";
    cout << "====================================================\n\n";

    cout << "  1. Truck Entry\n";
    cout << "  2. Truck Exit\n";
    cout << "  3. Search by Date\n";
    cout << "  4. Search by Truck Number\n";
    cout << "  5. Company-wise Records\n";
    cout << "  6. Currently Inside Trucks\n";
    cout << "  7. Show All Records\n";
    cout << "  8. Dashboard Summary\n";
    cout << "  9. Generate CSV + HTML Reports\n";
    cout << " 10. Open Premium HTML Dashboard\n";
    cout << " 11. Share Report on WhatsApp\n";
    cout << "  0. Exit\n";

    cout << "\n====================================================\n";
}

// ============================================================
// MAIN
// ============================================================

int main()
{
    loadRecords();

    generateCSV();
    generateHTML();

    cout << "\n";
    cout << "====================================================\n";
    cout << "      RELIABLE LOGISTICS SOLUTIONS PVT. LTD.\n";
    cout << "====================================================\n\n";

    cout <<
        "Existing records loaded: "
        << records.size()
        << "\n";

    cout <<
        "Permanent data file: "
        << DATA_FILE
        << "\n";

    cout <<
        "HTML dashboard: "
        << HTML_FILE
        << "\n";

    cout <<
        "CSV report: "
        << CSV_FILE
        << "\n";

    while (true)
    {
        showMenu();

        int choice =
            inputInt(
                "Enter your choice: "
            );

        switch (choice)
        {
            case 1:
                truckEntry();
                break;

            case 2:
                truckExit();
                break;

            case 3:
                searchByDate();
                break;

            case 4:
                searchByTruck();
                break;

            case 5:
                searchByCompany();
                break;

            case 6:
                showInsideTrucks();
                break;

            case 7:
                showRecords(records);
                break;

            case 8:
                showSummary();
                break;

            case 9:
                regenerateReports();
                break;

            case 10:
                openDashboard();
                break;

            case 11:
                shareWhatsApp();
                break;

            case 0:

                saveRecords();
                generateCSV();
                generateHTML();

                cout << "\n";
                cout <<
                    "All data saved successfully.\n";

                cout <<
                    "Thank you for using the system.\n";

                return 0;

            default:

                cout <<
                    "\nInvalid choice.\n";

                cout <<
                    "Please select 0-11.\n";
        }
    }

    return 0;
}