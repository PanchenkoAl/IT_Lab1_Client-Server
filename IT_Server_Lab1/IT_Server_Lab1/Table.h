#pragma once
// IT_Lab1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <variant>
#include <regex>
#include <complex>
#include <string>
#include <sstream>
#include <unordered_map>
#include <iomanip>
#include <fstream>

static int CURRENT_ID = 0;
using DataType = std::variant<int, float, std::complex<double>, std::string>;
using ColumnType = std::variant<std::vector<int>, std::vector<float>, std::vector<std::complex<double>>, std::vector<std::string>>;

class Table
{
    std::string  m_name;
    unsigned int m_id;
    unsigned int m_colsize;
    unsigned int m_rows = 0;

    //std::vector<std::vector<std::variant<int, float, std::string, >>> m_columns;
    std::vector<ColumnType> m_columns;
    std::vector<std::string> m_colNames;
    std::vector<std::string> m_dTypes;

public:
    Table(int id, int colsize, std::string name) :
        m_id(id),
        m_colsize(colsize),
        m_name(name)
    {}

    Table()
    {}

    std::vector<ColumnType> getColumns() const
    {
        return m_columns;
    }

    std::vector<std::string> getColNames() const
    {
        return m_colNames;
    }

    std::vector<std::string> getDataTypes() const
    {
        return m_dTypes;
    }

    void addColName(std::string value)
    {
        m_colNames.push_back(value);
    }

    void addDataType(std::string value)
    {
        m_dTypes.push_back(value);
    }

    template<typename T>
    void addColumn() {
        m_columns.emplace_back(std::vector<T>{});
    }

    template<typename T>
    void addElementToColumn(size_t columnIndex, const T& value) {
        if (columnIndex < m_columns.size() && std::holds_alternative<std::vector<T>>(m_columns[columnIndex])) {
            std::get<std::vector<T>>(m_columns[columnIndex]).push_back(value);
        }
        else {
            std::cerr << "Err 1!" << std::endl;
        }
    }

    void addElementToColumnAuto(size_t columnIndex, const DataType& value) {
        if (std::holds_alternative<int>(value) && std::holds_alternative<std::vector<int>>(m_columns[columnIndex])) {
            addElementToColumn<int>(columnIndex, std::get<int>(value));
        }
        else if (std::holds_alternative<float>(value) && std::holds_alternative<std::vector<float>>(m_columns[columnIndex])) {
            addElementToColumn<float>(columnIndex, std::get<float>(value));
        }
        else if (std::holds_alternative<std::string>(value) && std::holds_alternative<std::vector<std::string>>(m_columns[columnIndex])) {
            addElementToColumn<std::string>(columnIndex, std::get<std::string>(value));
        }
        else if (std::holds_alternative<std::complex<double>>(value) && std::holds_alternative<std::vector<std::complex<double>>>(m_columns[columnIndex])) {
            addElementToColumn<std::complex<double>>(columnIndex, std::get<std::complex<double>>(value));
        }
        else {
            addElementToColumn<std::string>(columnIndex, std::get<std::string>(value));
        }
        m_rows++;
    }

    template<typename T>
    void updateElementInColumn(size_t columnIndex, const T& value, size_t rowIndex) {
        if (columnIndex < m_columns.size() && std::holds_alternative<std::vector<T>>(m_columns[columnIndex])) {
            std::get<std::vector<T>>(m_columns[columnIndex])[rowIndex] = value;
        }
        else {
            std::cerr << "Err 1!" << std::endl;
        }
    }

    void updateElementInColumnAuto(size_t columnIndex, const DataType& value, size_t rowIndex) {
        if (std::holds_alternative<int>(value) && std::holds_alternative<std::vector<int>>(m_columns[columnIndex])) {
            updateElementInColumn<int>(columnIndex, std::get<int>(value), rowIndex);
        }
        else if (std::holds_alternative<float>(value) && std::holds_alternative<std::vector<float>>(m_columns[columnIndex])) {
            updateElementInColumn<float>(columnIndex, std::get<float>(value), rowIndex);
        }
        else if (std::holds_alternative<std::string>(value) && std::holds_alternative<std::vector<std::string>>(m_columns[columnIndex])) {
            updateElementInColumn<std::string>(columnIndex, std::get<std::string>(value), rowIndex);
        }
        else if (std::holds_alternative<std::complex<double>>(value) && std::holds_alternative<std::vector<std::complex<double>>>(m_columns[columnIndex])) {
            updateElementInColumn<std::complex<double>>(columnIndex, std::get<std::complex<double>>(value), rowIndex);
        }
        else {
            std::cerr << "Type mismatch or invalid column index!" << std::endl;
        }
    }

    /*template<typename T>
    void printColumn(size_t columnIndex) const {
        if (columnIndex < m_columns.size() && std::holds_alternative<std::vector<T>>(m_columns[columnIndex])) {
            const auto& column = std::get<std::vector<T>>(m_columns[columnIndex]);
            for (const auto& element : column) {
                std::cout << element << " ";
            }
            std::cout << std::endl;
        }
        else {
            std::cerr << "Err 1!" << std::endl;
        }
    }*/

    void initiateTable(const SOCKET& ClientSocket)
    {
        std::string s;
        std::string datatype;
        std::string commonMessage;
        char recvbuf[512];
        int recvbuflen = 512;
        char messageSize[512];
        std::string ms;

        int iterator = 0;
        for (int i = 0; i < m_colsize; i++)
        {
            s = "";  datatype = "";
            commonMessage = "Enter column " + std::to_string(i + 1) + " name: " + "\n";
            send(ClientSocket, commonMessage.c_str(), commonMessage.length(), 0);
            recv(ClientSocket, messageSize, recvbuflen, 0);
            recv(ClientSocket, recvbuf, recvbuflen, 0);
            for (size_t k = 0; k < std::stoi(ms.assign(messageSize)); k++)
                s += recvbuf[k];

            commonMessage = "Enter data type: ";
            send(ClientSocket, commonMessage.c_str(), commonMessage.length(), 0);
            recv(ClientSocket, messageSize, recvbuflen, 0);
            recv(ClientSocket, recvbuf, recvbuflen, 0);
            for (size_t k = 0; k < std::stoi(ms.assign(messageSize)); k++)
                datatype += recvbuf[k];

            switch (datatype[0])
            {
            case 'i':
                addColumn<int>();
                break;
            case 'r':
                addColumn<float>();
                break;
            case 'c':
                addColumn<std::complex<double>>();
                break;
            case 's':
                addColumn<std::string>();
                break;
            }

            m_colNames.push_back(s);
            m_dTypes.push_back(datatype);
        }
    }

    std::string detectType(const std::string& input, DataType& modinput)
    {
        std::regex intRegex("^[-+]?\\d+$");
        if (std::regex_match(input, intRegex))
        {
            modinput = std::stoi(input);
            return "int";
        }

        std::regex realRegex("^[-+]?\\d*\\.\\d+([eE][-+]?\\d+)?$");
        if (std::regex_match(input, realRegex))
        {
            modinput = std::stof(input);
            return "real";
        }

        std::regex complexRegex("^\\((-?\\d*\\.?\\d+),(-?\\d*\\.?\\d+)\\)$");
        std::smatch match;
        if (std::regex_match(input, match, complexRegex)) {
            double realPart = std::stod(match[1]);
            double imagPart = std::stod(match[2]);
            modinput = std::complex<double>(realPart, imagPart);
            return "complex";
        }

        modinput = input;
        return "string";
    }

    void updateRowManual(size_t rowId, SOCKET ClientSocket)
    {
        std::vector<DataType> row;
        DataType modified;
        std::string s = "";

        std::string commonMessage;
        char recvbuf[512];
        int recvbuflen = 512;
        char messageSize[512];
        std::string ms;

        for (int i = 0; i < m_colsize; i++)
        {
            s = "";
            commonMessage = "Enter " + m_colNames[i] + " with " + m_dTypes[0] + "type: ";
            send(ClientSocket, commonMessage.c_str(), commonMessage.length(), 0);
            recv(ClientSocket, messageSize, recvbuflen, 0);
            recv(ClientSocket, recvbuf, recvbuflen, 0);
            for (size_t k = 0; k < std::stoi(ms.assign(messageSize)); k++)
                s += recvbuf[k];
            //recv(ClientSocket, recvbuf, recvbuflen, 0);
            //s.assign(recvbuf);

            if (detectType(s, modified) == m_dTypes[i])
            {
                row.push_back(modified);
            }
            else
            {
                commonMessage = "Invalid input.\n";
                send(ClientSocket, commonMessage.c_str(), commonMessage.length(), 0);
                return;
            }
        }

        for (int i = 0; i < m_colsize; i++)
        {
            updateElementInColumnAuto(i, row[i], rowId);
        }
    }

    void addRowManual(SOCKET ClientSocket)
    {
        std::vector<DataType> row;
        DataType modified;
        std::string s = "";

        std::string commonMessage;
        char recvbuf[512];
        int recvbuflen = 512;
        char messageSize[512];
        std::string ms;

        for (int i = 0; i < m_colsize; i++)
        {
            s = "";
            commonMessage = "Enter " + m_colNames[i] + " with " + m_dTypes[i] + " type: ";
            send(ClientSocket, commonMessage.c_str(), commonMessage.length(), 0);
            recv(ClientSocket, messageSize, recvbuflen, 0);
            recv(ClientSocket, recvbuf, recvbuflen, 0);
            for (size_t k = 0; k < std::stoi(ms.assign(messageSize)); k++)
                s += recvbuf[k];

            /*recv(ClientSocket, recvbuf, recvbuflen, 0);
            s.assign(recvbuf);*/

            if (detectType(s, modified) == m_dTypes[i])
            {
                row.push_back(modified);
            }
            else
            {
                commonMessage = "Invalid input.\n";
                send(ClientSocket, commonMessage.c_str(), commonMessage.length(), 0);
                return;
            }
        }

        for (int i = 0; i < m_colsize; i++)
        {
            addElementToColumnAuto(i, row[i]);
        }
    }

    template <typename T>
    void printTable()
    {
        for (int j = 0; j < std::get<std::vector<T>>(m_columns[0]).size(); j++)
        {
            for (int i = 0; i < m_colsize; i++)
            {
                if (i < m_columns.size() && std::holds_alternative<std::vector<T>>(m_columns[i])) {
                    std::cout << std::get<std::vector<T>>(m_columns[i])[j] << " ";
                }
                else {
                    std::cerr << "Err 1!" << std::endl;
                }
                std::cout << std::endl;
            }
        }
    }

    size_t getMaxRows() const {
        size_t maxRows = 0;
        for (const auto& column : m_columns) {
            std::visit([&maxRows](const auto& col) {
               return col.size();
                }, column);
        }
        return m_rows;
    }

    void print(SOCKET ClientSocket) const 
    {
        std::string returnString = "\n";
        std::ostringstream oss;
        for (size_t i = 0; i < m_colNames.size(); i++)
        {
            oss << std::setw(15) << m_colNames[i];
        }
        std::string formattedString = oss.str() + "\n";

        returnString += formattedString;

        int numRows = 0;
        for (const auto& column : m_columns) {
            std::visit([&numRows](const auto& col) {
                numRows = col.size();
                }, column);
        }

        std::cout << numRows << std::endl;
        for (size_t row = 0; row < numRows; ++row)
        {
            oss.str(""); oss.clear();
            for (const auto& column : m_columns)
            {
                std::visit([row, &oss](const auto& col)
                    {
                        if (row < col.size())
                        {
                            oss << std::setw(15) << col[row];
                        }
                        else
                        {
                            oss << std::setw(15) << " ";
                        }
                    }, column);
            }
            formattedString = oss.str() + "\n";
            returnString += formattedString;
        }

        send(ClientSocket, returnString.c_str(), returnString.length(), 0);
    }

    int getColumnSize() const
    {
        return m_colsize;
    }
};

void saveToFile(const std::unordered_map<std::string, std::pair<Table, bool>>& DB, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Failed to open file for writing." << std::endl;
        return;
    }

    for (const auto& [tableName, tablePair] : DB) {
        if (!tablePair.second) continue;
        const Table& table = tablePair.first;

        file << tableName << "\n";
        file << table.getColumnSize() << "\n";

        for (int i = 0; i < table.getColumnSize(); ++i) {
            file << table.getColNames()[i] << " " << table.getDataTypes()[i] << "\n";
        }

        size_t numRows = table.getMaxRows();
        for (size_t row = 0; row < numRows; ++row) {
            for (const auto& column : table.getColumns()) {
                std::visit([&file, row](const auto& col) {
                    if (row < col.size()) {
                        file << col[row] << " ";
                    }
                    }, column);
            }
            file << "\n";
        }
        file << "---\n";
    }
    file.close();
    std::cout << "Database saved to " << filename << std::endl;
}

void loadFromFile(std::unordered_map<std::string, std::pair<Table, bool>>& DB, const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open file for reading." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::string tableName = line;
        int colAmount;
        file >> colAmount;
        file.ignore();

        Table table(++CURRENT_ID, colAmount, tableName);

        for (int i = 0; i < colAmount; ++i) {
            std::string colName, dataType;
            file >> colName >> dataType;
            table.addColName(colName);
            table.addDataType(dataType);

            if (dataType == "int") {
                table.addColumn<int>();
            }
            else if (dataType == "real") {
                table.addColumn<float>();
            }
            else if (dataType == "complex") {
                table.addColumn<std::complex<double>>();
            }
            else if (dataType == "string") {
                table.addColumn<std::string>();
            }
        }

        file.ignore();

        std::string row;
        while (std::getline(file, row) && row != "---") {
            std::istringstream rowStream(row);
            for (int i = 0; i < colAmount; ++i) {
                if (table.getDataTypes()[i] == "int") {
                    int val;
                    rowStream >> val;
                    table.addElementToColumn<int>(i, val);
                }
                else if (table.getDataTypes()[i] == "real") {
                    float val;
                    rowStream >> val;
                    table.addElementToColumn<float>(i, val);
                }
                else if (table.getDataTypes()[i] == "complex") {
                    std::complex<double> val;
                    rowStream >> val;
                    table.addElementToColumn<std::complex<double>>(i, val);
                }
                else if (table.getDataTypes()[i] == "string") {
                    std::string val;
                    rowStream >> val;
                    table.addElementToColumn<std::string>(i, val);
                }
            }
        }

        DB[tableName] = { table, true };
    }

    file.close();
    std::cout << "Database loaded from " << filename << std::endl;
}
