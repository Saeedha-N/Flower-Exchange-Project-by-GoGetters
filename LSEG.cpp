#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
// #include <iomanip> // Include the <iomanip> header for formatting

using namespace std;

ifstream input_file("Orders.csv");         // Replace "input.csv" with the input CSV file's name
ofstream output_file("Execution_Rep.csv"); // Replace "output.csv" with the output CSV file's name

struct Order
{
    string clientOrderID;
    string orderID;
    string instrument;
    int side;
    int status;
    int quantity;
    double price;
    string reason;

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
        ss << orderID << "," << clientOrderID << "," << instrument << "," << side << "," << status << "," << quantity << "," << price << "," << reason;
        return ss.str();
    }
};

struct entityOrder {
    string clientOrderID;
    int quantity;
    double price;
};

///////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////
class OrderBook
{
public:
    string instrument;
    vector<Order> orders;
    vector<entityOrder> buyOrders;
    vector<entityOrder> sellOrders;

    OrderBook(string instrument)
    {
        this->instrument = instrument;
    }
    ///////////////////////////////////////////
    void addOrder(Order order) {

         if (order.price < 0) {                          //CHECK QUANTITY AND PRICE DATA TYPES , ADD TIME STAMP
            order.reason = "Invalid price";
            order.status = 1; // Rejected
            output_file << order.toString() << endl;
            return;
        }

        if (order.quantity % 10 != 0) {
            order.reason = "Invalid size";
            order.status = 1; // Rejected
            output_file << order.toString() << endl;
            return;
        }

        if ((10 > order.quantity) || (order.quantity >  1000)) {
            order.reason = "Invalid size";
            order.status = 1; // Rejected
            output_file << order.toString() << endl;
            return;
        }

        if (order.side == 1 || order.side == 2) {
            completeTransaction(order);
        }

        else {
            order.reason = "Invalid side";
            order.status = 1; // Rejected
            output_file << order.toString() << endl;
        }
    }
    ///////////////////////////////////////////

    int findOrderById(vector<Order>& orders, string orderID) {
        for (int j = orders.size()-1 ; -1; j--) {
            if (orders[j].orderID == orderID) {
                return j;
            }
        }
        return -1;

    }   

    void completeTransaction(Order order) {
        if (order.side == 1 ) {
            // Buying transaction
            bool pfill = false;
            
            int i = 0;
            while (i < sellOrders.size()) {

                bool price = false;
                if (order.price == 2 and sellOrders[i].price == 1){
                    order.price = 1;
                    price = true;
                
                if (sellOrders[i].price == order.price or price)  {
                    int orderIndex = findOrderById(orders, sellOrders[i].clientOrderID);
                    Order SellerOrder = orders[orderIndex];
                    if (price)
                        SellerOrder.price = 1;

                    if (sellOrders[i].quantity >= order.quantity) {
                        order.status = 2; // Filled
                        orders.push_back(order);
                        output_file << order.toString() << endl;

                        if (sellOrders[i].quantity == order.quantity) {
                            SellerOrder.status = 2; // Filled
                            SellerOrder.quantity = order.quantity;
                            orders.push_back(SellerOrder);
                            output_file << SellerOrder.toString() << endl;
                            sellOrders.erase(sellOrders.begin() + i);
                        }
                        else {
                            SellerOrder.status = 3; // PFilled
                            SellerOrder.quantity = order.quantity;
                            orders.push_back(SellerOrder);
                            output_file << SellerOrder.toString() << endl;
                            sellOrders[i].quantity -= order.quantity;
                            
                        }

                    }
                    else {
                        int tmp = order.quantity - sellOrders[i].quantity;

                        if (buyOrders.size() > 0 and buyOrders.back().clientOrderID == order.orderID)
                            buyOrders.erase(buyOrders.end() - 1 );
                        buyOrders.push_back({ order.orderID, order.quantity - sellOrders[i].quantity, order.price });
                        
                        order.status = 3; // PFilled
                        order.quantity = sellOrders[i].quantity;
                        orders.push_back(order);
                        output_file << order.toString() << endl;
                        order.quantity = order.quantity - sellOrders[i].quantity;

                        SellerOrder.status = 2; // Filled
                        SellerOrder.quantity = sellOrders[i].quantity;
                        orders.push_back(SellerOrder);
                        output_file << SellerOrder.toString() << endl;
                        sellOrders.erase(sellOrders.begin() + i);
                        order.quantity = tmp;

                        i--;
                    }
                    pfill = true;
                }
                }
                i++;
            }
            if (pfill) {
				return;
            }
            
            
            // If no match add Buyer's order to the buyOrders vector
            entityOrder buyOrder = { order.orderID, order.quantity, order.price };
            buyOrders.push_back(buyOrder);
            orders.push_back(order); // Default status is 0 (New)
            output_file << order.toString() << endl;


        }
        else { 
            // Selling transaction
            bool pfill = false;
            bool price = false;
            if (order.price = 1) {
                bool price = true;
    
            
            int i = 0;
            while (i < buyOrders.size()) {

                if (price)
                    order.price = buyOrders[i].price;

                if (buyOrders[i].price == order.price) {


                    int orderIndex = findOrderById(orders, buyOrders[i].clientOrderID);
                    Order BuyerOrder = orders[orderIndex];

                    if (buyOrders[i].quantity >= order.quantity) {
                        
                        order.status = 2; // Filled
                        orders.push_back(order);
                        output_file << order.toString() << endl;

                        if (buyOrders[i].quantity == order.quantity) {
                            BuyerOrder.status = 2; // Filled
                            BuyerOrder.quantity = order.quantity;
                            orders.push_back(BuyerOrder);
                            output_file << BuyerOrder.toString() << endl;
                            buyOrders.erase(buyOrders.begin() + i);
                        }
                        else {
                            BuyerOrder.status = 3; // PFilled
                            BuyerOrder.quantity = order.quantity;
                            orders.push_back(BuyerOrder);
                            output_file << BuyerOrder.toString() << endl;
                            buyOrders[i].quantity -= order.quantity;
                        }

                    }
                    else {
                        int tmp = order.quantity - buyOrders[i].quantity;

                        order.status = 3; // PFilled
                        order.quantity = buyOrders[i].quantity;
                        orders.push_back(order);
                        output_file << order.toString() << endl;

                        if (price)
                            order.price = 1;

                        if ((sellOrders.size() > 0) and (sellOrders.back().clientOrderID ==  order.orderID))
                            sellOrders.erase(sellOrders.end() - 1);
                        sellOrders.push_back({ order.orderID, tmp, order.price });  // if exists need to update otherwise need to create new one

                        BuyerOrder.status = 2; // Filled
                        BuyerOrder.quantity = buyOrders[i].quantity;   // changed 
                        orders.push_back(BuyerOrder);
                        output_file << BuyerOrder.toString() << endl;
                        buyOrders.erase(buyOrders.begin() + i);
                        order.quantity = tmp;

                        if (price)
                            order.price = 1;
                        i--;
                    }
                    pfill = true;
                }
                i++;
            }
            if (pfill) {
                return;
            }
            }
            // If no match add Seller's order to the sellOrders vector 
            sellOrders.push_back({ order.orderID, order.quantity, order.price });
            orders.push_back(order); // Default status is 0 (New)
            output_file << order.toString() << endl;
        }
    }
};


///////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    // Create the order books.
    OrderBook roseOrderBook("Rose");
    OrderBook lavenderOrderBook("Lavender");
    OrderBook lotusOrderBook("Lotus");
    OrderBook tulipOrderBook("Tulip");
    OrderBook orchidOrderBook("Orchid");

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

    // Add the header row to the output file
    output_file << "execution_rep.csv" << endl;

    // Add the field names row to the output file
    output_file << "Order ID, Client Order ID, Instrument, Side, Execution Status, Quantity, Price, Reason" << endl;

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
        else
        {
            order.reason = "Invalid instrument";
            order.status = 1; // Rejected
            output_file << order.toString() << endl;
        }

        order_id++;
    }
    /////////////////////////////////////////////////////////////////////

    cout << "Data processed and written to Execution_Rep.csv" << endl;

    input_file.close();
    output_file.close();

    return 0;
}