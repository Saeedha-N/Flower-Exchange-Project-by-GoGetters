#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
// #include <iomanip> // Include the <iomanip> header for formatting

using namespace std;

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

        if (order.side == 1 || order.side == 2) {
            completeTransaction(order);
        }

        else {
            order.reason = "Invalid side";
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
        if (order.side == 1) {
            // Buying transaction if exact match is found
            // for (int i = 0; i < sellOrders.size(); i++) {
            //     if (sellOrders[i].price == order.price && sellOrders[i].quantity == order.quantity) {
            //         // Add Buyer's updated order to the orders vector
            //         order.status = 2; // Filled
            //         orders.push_back(order);
// 
            //         // Find seller's order in the orders vector and add a new order to the orders vector
            //         int orderIndex = findOrderById(orders, sellOrders[i].clientOrderID);
            //         Order SellerOrder = orders[orderIndex];
            //         SellerOrder.status = 2; // Filled
            //         //SellerOrder.quantity = order.quantity;   //Add this line
            //         orders.push_back(SellerOrder);   
            //         sellOrders.erase(sellOrders.begin() + i);
            //         return;
            //     }
            // }

            // Buying transaction if only price match is found
            bool pfill = false;
            for (int i = 0; i < sellOrders.size(); i++) {
                
                if (sellOrders[i].price == order.price) {


                    int orderIndex = findOrderById(orders, sellOrders[i].clientOrderID);
                    Order SellerOrder = orders[orderIndex];

                    if (sellOrders[i].quantity >= order.quantity) {
                        order.status = 2; // Filled
                        orders.push_back(order);

                        if (sellOrders[i].quantity == order.quantity) {
                            SellerOrder.status = 2; // Filled
                            SellerOrder.quantity = order.quantity;
                            orders.push_back(SellerOrder);
                            sellOrders.erase(sellOrders.begin() + i);
                        }
                        else {
                            SellerOrder.status = 3; // PFilled
                            SellerOrder.quantity = order.quantity;
                            orders.push_back(SellerOrder);
                            sellOrders[i].quantity -= order.quantity;
                            
                        }

                    }
                    else {
                        int tmp = order.quantity - sellOrders[i].quantity;
                        buyOrders.push_back({ order.orderID, order.quantity - sellOrders[i].quantity, order.price });
                        
                        order.status = 3; // PFilled
                        order.quantity = sellOrders[i].quantity;
                        orders.push_back(order);
                        order.quantity = order.quantity - sellOrders[i].quantity;

                        SellerOrder.status = 2; // Filled
                        SellerOrder.quantity = sellOrders[i].quantity;
                        orders.push_back(SellerOrder);
                        sellOrders.erase(sellOrders.begin() + i);
                        order.quantity = tmp;
                        i--;
                    }
                    pfill = true;
                }
            }
            if (pfill) {
				return;
			}

            // If no match add Buyer's order to the buyOrders vector
            entityOrder buyOrder = { order.orderID, order.quantity, order.price };
            buyOrders.push_back(buyOrder);
            orders.push_back(order); // Default status is 0 (New)


        }
        else { //order.side == 2
            // Selling transaction if exact match is found
            // for (int i = 0; i < buyOrders.size(); i++) {
            //     if (buyOrders[i].price == order.price && buyOrders[i].quantity == order.quantity) {
            //         // Add Seller's order to the orders vector
            //         order.status = 2; // Filled
            //         orders.push_back(order);
// 
            //         // Find Buyer's order in the orders vector and add a new order to the orders vector
            //         int orderIndex = findOrderById(orders, buyOrders[i].clientOrderID);
            //         Order BuyerOrder = orders[orderIndex];
            //         BuyerOrder.status = 2; // Filled
            //         orders.push_back(BuyerOrder);
            //         buyOrders.erase(buyOrders.begin() + i);
            //         return;
            //     }
            // }

            // Selling transaction if only price match is found
            bool pfill = false;
            int i = 0;
            while (i < buyOrders.size()) {
                

                if (buyOrders[i].price == order.price) {


                    int orderIndex = findOrderById(orders, buyOrders[i].clientOrderID);
                    Order BuyerOrder = orders[orderIndex];

                    if (buyOrders[i].quantity >= order.quantity) {
                        
                        order.status = 2; // Filled
                        orders.push_back(order);
                        

                        if (buyOrders[i].quantity == order.quantity) {
                            BuyerOrder.status = 2; // Filled
                            BuyerOrder.quantity = order.quantity;
                            orders.push_back(BuyerOrder);
                            buyOrders.erase(buyOrders.begin() + i);
                        }
                        else {
                            BuyerOrder.status = 3; // PFilled
                            BuyerOrder.quantity = order.quantity;
                            orders.push_back(BuyerOrder);
                            buyOrders[i].quantity -= order.quantity;
                        }

                    }
                    else {
                        int tmp = order.quantity - buyOrders[i].quantity;
 
                        sellOrders.push_back({ order.orderID, order.quantity - buyOrders[i].quantity, order.price });
                        
                        order.status = 3; // PFilled
                        order.quantity = buyOrders[i].quantity;
                        orders.push_back(order);
                        


                        BuyerOrder.status = 2; // Filled
                        BuyerOrder.quantity = sellOrders[i].quantity;
                        orders.push_back(BuyerOrder);
                        buyOrders.erase(buyOrders.begin() + i);
                        order.quantity = tmp;
                        i--;
                    }
                    pfill = true;
                }
                i++;
            }
            if (pfill) {
                return;
            }

            // If no match add Seller's order to the sellOrders vector 
            sellOrders.push_back({ order.orderID, order.quantity, order.price });
            orders.push_back(order); // Default status is 0 (New)
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
    /////////////////////////////////////////////////////////////////////

    input_file.close();

    // Add the header row to the output file
    output_file << "execution_rep.csv" << endl;

    // Add the field names row to the output file
    output_file << "Order ID, Client Order ID, Instrument, Side, Execution Status, Quantity, Price, Reason" << endl;

    // Print the orders in each order book.
    for (OrderBook orderBook : {roseOrderBook, lavenderOrderBook, lotusOrderBook, tulipOrderBook, orchidOrderBook})
    {
        for (Order order : orderBook.orders)
        {
            output_file << order.toString() << endl;
        }
    }

    output_file.close();

    cout << "Data processed and written to Execution_Rep.csv" << endl;

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////



    //     else {
    //         // Selling transaction
    //         for (int i = 0; i < buyOrders.size(); i++) {
    //             if (buyOrders[i].price == order.price && buyOrders[i].quantity == order.quantity) {
    //                 int status = 2; // Filled

    //                 // Add Seller's order to the orders vector
    //                 order.status = status;
    //                 orders.push_back(order);

    //                 // Find Buyer's order in the orders vector and add a new order to the orders vector
    //                 for (int j = 0; j < orders.size(); j++) {
    //                     if (orders[j].orderID == buyOrders[i].clientOrderID) {
    //                         Order pairBuyerOrder = orders[j];
    //                         pairBuyerOrder.status = status;
    //                         orders.push_back(pairBuyerOrder);
    //                         break;
    //                     }
    //                 }
    //                 return;
    //             }
    //         }

    //         // If no match add Seller's order to the sellOrders vector
    //         sellOrders.push_back({order.orderID, order.quantity, order.price});
    //         orders.push_back(order); //Default status is 0 (New)
    //     }
    // }     