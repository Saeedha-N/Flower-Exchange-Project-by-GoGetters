#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip> // Include the <iomanip> header for formatting

using namespace std;

class Order
{
public:
    string clientOrderID;
    string orderID;
    string instrument;
    int side;
    int status;
    int quantity;
    double price;

    // According to execution report format
    Order(string orderID, string clientOrderID, string instrument, int side, int status, int quantity, double price)
    {
        this->orderID = orderID;
        this->clientOrderID = clientOrderID;
        this->instrument = instrument;
        this->side = side;
        this->status = status;
        this->quantity = quantity;
        this->price = price;
    }

    string toString()
    {
        stringstream ss;
        ss << orderID << "," << clientOrderID << "," << instrument << "," << side << "," << status << "," << quantity << "," << fixed << setprecision(2) << price;
        return ss.str();
    }
};

class OrderBook
{
public:
    string instrument;
    vector<Order> orders;

    OrderBook(string instrument)
    {
        this->instrument = instrument;
    }

    void addOrder(Order order)
    {
        orders.push_back(order);
    }

    void printOrders()
    {
        for (Order order : orders)
        {
            cout << order.toString() << endl;
        }
    }
};

int main()
{
    // Create the order books.
    OrderBook roseOrderBook("Rose");
    OrderBook lavenderOrderBook("Lavender");
    OrderBook lotusOrderBook("Lotus");
    OrderBook tulipOrderBook("Tulip");
    OrderBook orchidOrderBook("Orchid");

    ifstream input_file("Orders.csv");         // Replace "input.csv" with the input CSV file's name
    ofstream output_file("Execution_Rep.csv"); // Replace "output.csv" with the output CSV file's name

    if (!input_file.is_open()) // If the input file isn't open, return an error
    {
        cerr << "Could not open the input file." << endl; // cerr is like cout but for errors
        return 1;
    }

    if (!output_file.is_open()) // If the output file isn't open, return an error
    {
        cerr << "Could not create or open the output file." << endl;
        return 1;
    }

    /////////////////////////////////////////////////////////////////
    string line;
    getline(input_file, line); // Skip the header row
    getline(input_file, line); // Skip the field names row
    int order_id = 1;

    while (getline(input_file, line))
    {
        vector<string> row;

        istringstream iss(line); // Treats a string like a stream (like cin) allowing to get data
        string field;

        while (getline(iss, field, ',')) // Read a string until next comma and put it in 'field'
        {
            row.push_back(field); // Add the field to the end of the row
        }

        string order_id_string = "ord" + to_string(order_id); // Create the order ID string
        int status = 0;                                       // 0 = New, 1 = Rejected, 2 = Filled, 3 = Partially Filled

        cout << row[0] << " " << row[1] << " " << row[2] << " " << row[3] << " " << row[4] << endl;

        // Create an order
        Order order(order_id_string, row[0], row[1], stoi(row[2]), status, stoi(row[3]), stod(row[4]));

        // Add the order to the appropriate order book
        if (row[1] == "Rose")
        {
            roseOrderBook.addOrder(order);
        }
        else if (row[1] == "Lavender")
        {
            lavenderOrderBook.addOrder(order);
        }
        else if (row[1] == "Lotus")
        {
            lotusOrderBook.addOrder(order);
        }
        else if (row[1] == "Tulip")
        {
            tulipOrderBook.addOrder(order);
        }
        else if (row[1] == "Orchid")
        {
            orchidOrderBook.addOrder(order);
        }

        order_id++;
    }

    input_file.close();

    // Add the header row to the output file
    output_file << "execution_rep.csv" << endl;

    // Add the field names row to the output file
    output_file << "Order ID, Client Order ID, Instrument, Side, Execution Status, Quantity, Price" << endl;

    // Print the orders in each order book.
    for (OrderBook orderBook : {roseOrderBook, lavenderOrderBook, lotusOrderBook, tulipOrderBook, orchidOrderBook})
    {
        for (Order order : orderBook.orders)
        {
            output_file << order.toString() << endl;
        }
    }

    // Process the data if needed
    // int item_number = 1;

    // for (const auto &row : data) // For each row in the data vector
    // {
    //     output_file << "ord" << item_number << ",";

    //     for (size_t i = 0; i < row.size(); ++i)
    //     {
    //         output_file << row[i];
    //         if (i < row.size() - 1)
    //         {
    //             output_file << ",";
    //         }
    //     }
    //     output_file << endl;
    //     item_number++;
    // }

    output_file.close();

    cout << "Data processed and written to Execution_Rep.csv" << endl;

    return 0;
}
