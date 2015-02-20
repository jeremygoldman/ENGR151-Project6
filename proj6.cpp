#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;
//author: Jeremy Goldman

//purpose: simulate a stock market, use different strategies of 
//  buying/selling securities to see which makes more profit given 
//  a list of transactions

class Order {
public:
	//default constructor
	Order() {
		symbol = "";
		price = 0;
		buyOrSell = 0;
	}
	//constructor accepting symbol, buying or selling, and price of security.
	Order(string symbol, int buyOrSell, int price) {
		this->symbol = symbol;
		this->buyOrSell = buyOrSell;
		this->price = price;
	}
	//returns price of security
	int GetPrice() {
		return price;
	}
	//returns true if order is for buying a security, false for if selling
	bool IsBuy() {
		if (buyOrSell > 0) {
			return true;
		} else {
			return false;
		}
	}
	//returns 0 is symbol is B, 1 if F, and 2 if I
	int GetSymbol() {
		if (symbol == "B") {
			return 0;
		} else if (symbol == "F") {
			return 1;
		} else {
			return 2;
		}
	}
	//returns the letter form of the symbol
	string GetLetterSymbol() {
		return symbol;
	}
	//overrides the == operator to check if the symbol, price, and 
	//  buy/sell of this order matches the values of the order it is 
	//  being compared to. Returns true if values are the same, false if 
	//  values are not all the same.
	bool operator==(Order rhs) {
		if (this->GetSymbol() == rhs.GetSymbol() && price == rhs.GetPrice() 
			&& (this->IsBuy() == rhs.IsBuy())) {
			return true;
		} else {
			return false;
		}
	}
private:
	string symbol;
	int price;
	int buyOrSell;
};






int main() {
	//first task: generate sequence of orders
	ifstream orderFS;
	string orderFileName;
	cin >> orderFileName;
	int mmSpreadThreshold;
	cin >> mmSpreadThreshold;
	orderFS.open(orderFileName.c_str());
	
	//create buy order and sell order vectors for each symbol
	vector<Order> bBuyOrders;
	vector<Order> bSellOrders;
	vector<Order> fBuyOrders;
	vector<Order> fSellOrders;
	vector<Order> iBuyOrders;
	vector<Order> iSellOrders;

	Order currentbBuyOrder = Order("B", 1, 0);
	Order currentbSellOrder = Order("B", -1, 0);
	Order currentfBuyOrder = Order("F", 1, 0);
	Order currentfSellOrder = Order("F", -1, 0);
	Order currentiBuyOrder = Order("I", 1, 0);
	Order currentiSellOrder = Order("I", -1, 0);

	vector<Order> mmbBuyOrders;
	vector<Order> mmbSellOrders;
	vector<Order> mmfBuyOrders;
	vector<Order> mmfSellOrders;
	vector<Order> mmiBuyOrders;
	vector<Order> mmiSellOrders;

	int soldbOrders = 0;
	int boughtbOrders = 0;
	int soldfOrders = 0;
	int boughtfOrders = 0;
	int soldiOrders = 0;
	int boughtiOrders = 0;

	int bPrice = 0;
	int fPrice = 0;
	int iPrice = 0;

	int totalMMcash = 0;
	
	//get orders
	string currentSymbol = "";
	string nextSymbol = "";
	int currentBuyOrSell = 0;
	int currentPrice = 0;
	int i = 0;
	orderFS >> currentSymbol;
	int totaliaProfit = 0;
	while (orderFS.good()) {
		//get information about order
		orderFS >> currentBuyOrSell;
		orderFS >> currentPrice;
		/*
		cout << "current symbol: " << currentSymbol << " ";
		cout << "current BuyOrSell: " << currentBuyOrSell << " ";
		cout << "current Price: " << currentPrice << " " << endl;
		*/
		if (!orderFS.eof()) {
			Order currentOrder = Order(currentSymbol, currentBuyOrSell, currentPrice);
			bool foundMatch = false;
			switch(currentOrder.GetSymbol()) {
				case (0):
					i = 0;
					if (currentOrder.IsBuy()) {
						//cout << "its a buy" << endl;
						//current order is for a buy
						
						while (!foundMatch && i<bSellOrders.size()) {
							if (currentOrder.GetPrice() >= bSellOrders[i].GetPrice()) {
								//cout << "transaction occurs" << endl;
								//transaction occurs at price of sell order
								int agreedPrice = bSellOrders[i].GetPrice();
								//remove sell order from sell order index
								bSellOrders.erase(bSellOrders.begin() + i);
								bool mmSell = false;
								if (mmbSellOrders.size() == 0) {
									mmSell = false;
								} else {
									int u = 0;
									bool found = false;
									while (!found && u<bSellOrders.size()) {
										if (mmbSellOrders[0] == bSellOrders[u]) {
											found = true;
										} else {
											u++;
										}
									}
									if (!found) {
										mmSell = true;
									}
								}
								//output transaction to ticker
								cout << currentOrder.GetLetterSymbol() << ": " << agreedPrice;
								bPrice = agreedPrice;
								if (mmSell) {
									cout << " (MM sell)";
									totalMMcash += agreedPrice;
									soldbOrders++;
									mmbSellOrders.clear();
									currentbSellOrder = Order("B", -1, 0);
								}
								cout << endl;

								foundMatch = true;
							} else {
								//havent found match yet, so move onto next index in bBuyOrders array
								i++;
							}
						}
						if (!foundMatch) {
							//cout << "no transaction, goes in book" << endl;
							//transaction doesn't occur
							//go through buy orders, put current order in, make sure it's 
							// DESCENDING price order
							bool foundLower = false;
							int j = 0;
							while (!foundLower && j<bBuyOrders.size()) {
								if (currentOrder.GetPrice() > bBuyOrders.at(j).GetPrice()) {
									foundLower = true;
								} else {
									j++;
								}
							}
							//cout << "j = "<< j << endl;
							//current order has lowest price
							if (j == bBuyOrders.size() && !foundLower) {
								//cout << "has lowest price" << endl;
								bBuyOrders.push_back(currentOrder);
							} else {
								//cout << "inserting at index " << j << endl;
								bBuyOrders.insert(bBuyOrders.begin()+j, currentOrder);
							}
						//order has now been inserted into vector of buy orders
						}
					} else {
						//cout << "its a sell" << endl;
						//current order is for a sell
			  			while (!foundMatch && i<bBuyOrders.size()) {
							if (currentOrder.GetPrice() <= bBuyOrders[i].GetPrice()) {
								//transaction occurs at price of sell order
								int agreedPrice = bBuyOrders[i].GetPrice();
								//remove buy order from buy order index
								bBuyOrders.erase(bBuyOrders.begin() + i);
								//AAAAAAAAA
								bool mmSell = false;
								if (mmbBuyOrders.size() == 0) {
									mmSell = false;
								} else {
									int u = 0;
									bool found = false;
									while (!found && u<bBuyOrders.size()) {
										if (mmbBuyOrders[0] == bBuyOrders[u]) {
											found = true;
										} else {
											u++;
										}
									}
									if (!found) {
										mmSell = true;
									}
								}
								//output transaction to ticker
								cout << currentOrder.GetLetterSymbol() << ": " << agreedPrice;
								bPrice = agreedPrice;
								if (mmSell) {
									cout << " (MM buy)";
									totalMMcash -= agreedPrice;
									boughtbOrders++;
									mmbBuyOrders.clear();
									currentbBuyOrder = Order("B", 1, 0);
								}
								cout << endl;
								//AAAAAAAAA
								foundMatch = true;
							} else {
								//havent found match yet, so move onto next index in buyOrders array
								i++;
							}
						}
						if (!foundMatch) {
							//transaction doesn't occur
							//go through sell orders, put current order in, make sure it's 
							//  ASCENDING price order
							bool foundHigher = false;
							int j = 0;
							while (!foundHigher && j<bSellOrders.size()) {
								if (currentOrder.GetPrice() < bSellOrders.at(j).GetPrice()) {
									foundHigher = true;
								} else {
									j++;
								}
							}
							//current order has highest price
							if (j == bSellOrders.size()) {
								bSellOrders.push_back(currentOrder);
							} else {
								bSellOrders.insert(bSellOrders.begin()+j, currentOrder);
							}
						//order has now been inserted into vector of sell orders
						}
					}
					break;

				case (1):
					i = 0;
					if (currentOrder.IsBuy()) {
						//cout << "its a buy" << endl;
						//current order is for a buy
						
						while (!foundMatch && i<fSellOrders.size()) {
							if (currentOrder.GetPrice() >= fSellOrders[i].GetPrice()) {
								//cout << "transaction occurs" << endl;
								//transaction occurs at price of sell order
								int agreedPrice = fSellOrders[i].GetPrice();
								//remove sell order from sell order index
								fSellOrders.erase(fSellOrders.begin() + i);
								//AAAAAAAA
								bool mmSell = false;
								if (mmfSellOrders.size() == 0) {
									mmSell = false;
								} else {
									int u = 0;
									bool found = false;
									while (!found && u<fSellOrders.size()) {
										if (mmfSellOrders[0] == fSellOrders[u]) {
											found = true;
										} else {
											u++;
										}
									}
									if (!found) {
										mmSell = true;
									}
								}
								//output transaction to ticker
								cout << currentOrder.GetLetterSymbol() << ": " << agreedPrice;
								fPrice = agreedPrice;
								if (mmSell) {
									cout << " (MM sell)";
									totalMMcash += agreedPrice;
									soldfOrders++;
									mmfSellOrders.clear();
									currentfSellOrder = Order("F", -1, 0);
								}
								cout << endl;
								//AAAAAAAA
								foundMatch = true;
							} else {
								//havent found match yet, so move onto next index in buyOrders array
								i++;
							}
						}
						if (!foundMatch) {
							//cout << "no transaction, goes in book" << endl;
							//transaction doesn't occur
							//go through buy orders, put current order in, make sure it's 
							//  DESCENDING price order
							bool foundLower = false;
							int j = 0;
							while (!foundLower && j<fBuyOrders.size()) {
								if (currentOrder.GetPrice() > fBuyOrders.at(j).GetPrice()) {
									foundLower = true;
								} else {
									j++;
								}
							}
							//cout << "j = "<< j << endl;
							//current order has lowest price
							if (j == fBuyOrders.size() && !foundLower) {
								//cout << "has lowest price" << endl;
								fBuyOrders.push_back(currentOrder);
							} else {
								//cout << "inserting at index " << j << endl;
								fBuyOrders.insert(fBuyOrders.begin()+j, currentOrder);
							}
						//order has now been inserted into vector of buy orders
						}
					} else {
						//cout << "its a sell" << endl;
						//current order is for a sell
			  			while (!foundMatch && i<fBuyOrders.size()) {
							if (currentOrder.GetPrice() <= fBuyOrders[i].GetPrice()) {
								//transaction occurs at price of sell order
								int agreedPrice = fBuyOrders[i].GetPrice();
								
								//remove buy order from buy order index
								fBuyOrders.erase(fBuyOrders.begin() + i);
								//AAAAAAAA
								bool mmSell = false;
								if (mmfBuyOrders.size() == 0) {
									mmSell = false;
								} else {
									int u = 0;
									bool found = false;
									while (!found && u<fBuyOrders.size()) {
										if (mmfBuyOrders[0] == fBuyOrders[u]) {
											found = true;
										} else {
											u++;
										}
									}
									if (!found) {
										mmSell = true;
									}
								}
								//output transaction to ticker
								cout << currentOrder.GetLetterSymbol() << ": " << agreedPrice;
								fPrice = agreedPrice;
								if (mmSell) {
									cout << " (MM buy)";
									totalMMcash -= agreedPrice;
									boughtfOrders++;
									mmfBuyOrders.clear();
									currentfBuyOrder = Order("F", 1, 0);
								}
								cout << endl;
								//AAAAAAAAAA
								foundMatch = true;
							} else {
								//havent found match yet, so move onto next index in buyOrders array
								i++;
							}
						}
						if (!foundMatch) {
							//transaction doesn't occur
							//go through sell orders, put current order in, make sure it's 
							//  ASCENDING price order
							bool foundHigher = false;
							int j = 0;
							while (!foundHigher && j<fSellOrders.size()) {
								if (currentOrder.GetPrice() < fSellOrders.at(j).GetPrice()) {
									foundHigher = true;
								} else {
									j++;
								}
							}
							//current order has highest price
							if (j == fSellOrders.size()) {
								fSellOrders.push_back(currentOrder);
							} else {
								fSellOrders.insert(fSellOrders.begin()+j, currentOrder);
							}
						//order has now been inserted into vector of sell orders
						}
					}
					break;
				
				case (2):
					i = 0;
					if (currentOrder.IsBuy()) {
						//cout << "its a buy" << endl;
						//current order is for a buy
						
						while (!foundMatch && i<iSellOrders.size()) {
							if (currentOrder.GetPrice() >= iSellOrders[i].GetPrice()) {
								//cout << "transaction occurs" << endl;
								//transaction occurs at price of sell order
								int agreedPrice = iSellOrders[i].GetPrice();
								//remove sell order from sell order index
								iSellOrders.erase(iSellOrders.begin() + i);
								//AAAAAAAA
								bool mmSell = false;
								if (mmiSellOrders.size() == 0) {
									mmSell = false;
								} else {
									int u = 0;
									bool found = false;
									while (!found && u<iSellOrders.size()) {
										if (mmiSellOrders[0] == iSellOrders[u]) {
											found = true;
										} else {
											u++;
										}
									}
									if (!found) {
										mmSell = true;
									}
								}
								//output transaction to ticker
								cout << currentOrder.GetLetterSymbol() << ": " << agreedPrice;
								iPrice = agreedPrice;
								if (mmSell) {
									cout << " (MM sell)";
									totalMMcash += agreedPrice;
									soldiOrders++;
									mmiSellOrders.clear();
									currentiSellOrder = Order("I", -1, 0);
								}
								cout << endl;
								//AAAAAAAAA
								foundMatch = true;
							} else {
								//havent found match yet, so move onto next index in buyOrders array
								i++;
							}
						}
						if (!foundMatch) {
							//cout << "no transaction, goes in book" << endl;
							//transaction doesn't occur
							//go through buy orders, put current order in, make sure it's 
							//  DESCENDING price order
							bool foundLower = false;
							int j = 0;
							while (!foundLower && j<iBuyOrders.size()) {
								if (currentOrder.GetPrice() > iBuyOrders.at(j).GetPrice()) {
									foundLower = true;
								} else {
									j++;
								}
							}
							//cout << "j = "<< j << endl;
							//current order has lowest price
							if (j == iBuyOrders.size() && !foundLower) {
								//cout << "has lowest price" << endl;
								iBuyOrders.push_back(currentOrder);
							} else {
								//cout << "inserting at index " << j << endl;
								iBuyOrders.insert(iBuyOrders.begin()+j, currentOrder);
							}
						//order has now been inserted into vector of buy orders
						}
					} else {
						//cout << "its a sell" << endl;
						//current order is for a sell
			  			while (!foundMatch && i<iBuyOrders.size()) {
							if (currentOrder.GetPrice() <= iBuyOrders[i].GetPrice()) {
								//transaction occurs at price of sell order
								int agreedPrice = iBuyOrders[i].GetPrice();
								//remove buy order from buy order index
								iBuyOrders.erase(iBuyOrders.begin() + i);
								//AAAAAAAA
								bool mmSell = false;
								if (mmiBuyOrders.size() == 0) {
									mmSell = false;
								} else {
									int u = 0;
									bool found = false;
									while (!found && u<iBuyOrders.size()) {
										if (mmiBuyOrders[0] == iBuyOrders[u]) {
											found = true;
										} else {
											u++;
										}
									}
									if (!found) {
										mmSell = true;
									}
								}
								//output transaction to ticker
								cout << currentOrder.GetLetterSymbol() << ": " << agreedPrice;
								iPrice = agreedPrice;
								if (mmSell) {
									cout << " (MM buy)";
									totalMMcash -= agreedPrice;
									boughtiOrders++;
									mmiBuyOrders.clear();
									currentiBuyOrder = Order("I", 1, 0);
								}
								cout << endl;
								//AAAAAA
								foundMatch = true;
							} else {
								//havent found match yet, so move onto next index in buyOrders array
								i++;
							}
						}
						if (!foundMatch) {
							//transaction doesn't occur
							//go through sell orders, put current order in, make sure it's 
							//  ASCENDING price order
							bool foundHigher = false;
							int j = 0;
							while (!foundHigher && j<iSellOrders.size()) {
								if (currentOrder.GetPrice() < iSellOrders.at(j).GetPrice()) {
									foundHigher = true;
								} else {
									j++;
								}
							}
							//current order has highest price
							if (j == iSellOrders.size()) {
								iSellOrders.push_back(currentOrder);
							} else {
								iSellOrders.insert(iSellOrders.begin()+j, currentOrder);
							}
						//order has now been inserted into vector of sell orders
						}
					}
					break;
			}
		}
		/*
		cout << "current buy order book for B:";
		for (int i = 0; i<bBuyOrders.size(); i++) {
			cout << " " << bBuyOrders.at(i).GetPrice();
		}

		cout << endl << "current sell order book for B:";
		for (int i = 0; i<bSellOrders.size(); i++) {
			cout << " " << bSellOrders.at(i).GetPrice();
		}
		cout << endl;

		cout << "current buy order book for F:";
		for (int i = 0; i<fBuyOrders.size(); i++) {
			cout << " " << fBuyOrders.at(i).GetPrice();
		}

		cout << endl << "current sell order book for F:";
		for (int i = 0; i<fSellOrders.size(); i++) {
			cout << " " << fSellOrders.at(i).GetPrice();
		}
		cout << endl;

		cout << "current buy order book for I:";
		for (int i = 0; i<iBuyOrders.size(); i++) {
			cout << " " << iBuyOrders.at(i).GetPrice();
		}

		cout << endl << "current sell order book for I:";
		for (int i = 0; i<iSellOrders.size(); i++) {
			cout << " " << iSellOrders.at(i).GetPrice();
		}
		cout << endl;
		*/
		


		//task 2: implement IA strategy
		int bBuyQuote = 0;
		if (bBuyOrders.size() > 0)
			bBuyQuote = bBuyOrders[0].GetPrice();

		int bSellQuote = 0;
		if (bSellOrders.size() > 0)
			bSellQuote = bSellOrders[0].GetPrice();

		int fBuyQuote = 0;
		if (fBuyOrders.size() > 0)
			fBuyQuote = fBuyOrders[0].GetPrice();

		int fSellQuote = 0;
		if (fSellOrders.size() > 0)
			fSellQuote = fSellOrders[0].GetPrice();

		int iBuyQuote = 0;
		if (iBuyOrders.size() > 0)
			iBuyQuote = iBuyOrders[0].GetPrice();

		int iSellQuote = 0;
		if (iSellOrders.size() > 0)
			iSellQuote = iSellOrders[0].GetPrice();


		int iaProfit = 0;
		if (bSellQuote + fSellQuote < iBuyQuote && bSellQuote != 0 
			&& fSellQuote != 0 && iBuyQuote != 0) {
			//if buying 1 of both cereals would be less than the price 
			//  of selling one of the index
			
			/*
			cout << "make order: " << endl;
			cout << "B 1 " << bSellQuote << endl;
			cout << "F 1 " << fSellQuote << endl;
			cout << "I -1 " << iBuyQuote << endl << endl;
			*/
			iaProfit = iBuyQuote - bSellQuote - fSellQuote;
			totaliaProfit += iaProfit;
			cout << "B: " << bSellQuote;
			bPrice = bSellQuote;
			bSellOrders.erase(bSellOrders.begin());
			bool mmSell = false;
			if (mmbBuyOrders.size() == 0) {
				mmSell = false;
			} else {
				int u = 0;
				bool found = false;
				while (!found && u<bBuyOrders.size()) {
					if (mmbSellOrders[0] == bSellOrders[u]) {
						found = true;
					} else {
						u++;
					}
				}
				if (!found) {
					mmSell = true;
				}
			}
			//output transaction to ticker
			if (mmSell) {
				cout << " (MM sell)";
				totalMMcash += bSellQuote;
				soldbOrders++;
				mmbSellOrders.clear();
				currentbSellOrder = Order("B", 1, 0);
			}
			cout << endl;

			cout << "F: " << fSellQuote;
			fPrice = fSellQuote;
			fSellOrders.erase(fSellOrders.begin());
			mmSell = false;
			if (mmfBuyOrders.size() == 0) {
				mmSell = false;
			} else {
				int u = 0;
				bool found = false;
				while (!found && u<fBuyOrders.size()) {
					if (mmfSellOrders[0] == fSellOrders[u]) {
						found = true;
					} else {
						u++;
					}
				}
				if (!found) {
					mmSell = true;
				}
			}
			//output transaction to ticker
			if (mmSell) {
				cout << " (MM sell)";
				totalMMcash += fSellQuote;
				soldfOrders++;
				mmfSellOrders.clear();
				currentfSellOrder = Order("I", 1, 0);
			}
			cout << endl;

			cout << "I: " << iBuyQuote;
			iPrice = iBuyQuote;
			iBuyOrders.erase(iBuyOrders.begin());
			mmSell = false;
			if (mmiBuyOrders.size() == 0) {
				mmSell = false;
			} else {
				int u = 0;
				bool found = false;
				while (!found && u<iBuyOrders.size()) {
					if (mmiBuyOrders[0] == iBuyOrders[u]) {
						found = true;
					} else {
						u++;
					}
				}
				if (!found) {
					mmSell = true;
				}
			}
			//output transaction to ticker
			if (mmSell) {
				cout << " (MM buy)";
				totalMMcash -= iBuyQuote;
				boughtiOrders++;
				mmiBuyOrders.clear();
				currentiBuyOrder = Order("I", 1, 0);
			}
			cout << endl;

			cout << "Index Arbitrage: profit = " << iaProfit << endl;
			
		} else if (bBuyQuote + fBuyQuote > iSellQuote && bBuyQuote != 0 
			&& fBuyQuote != 0 && iSellQuote != 0) {
			//if selling 1 of both cereals would be more than the 
			//  price of buying one of the index
			
			/*
			cout << "make order: " << endl;
			cout << "B -1 " << bBuyQuote << endl;
			cout << "F -1 " << fBuyQuote << endl;
			cout << "I 1 " << iSellQuote << endl << endl;
			*/
			iaProfit = bBuyQuote + fBuyQuote - iSellQuote;
			totaliaProfit += iaProfit;

			cout << "B: " << bBuyQuote;
			bPrice = bBuyQuote;
			bBuyOrders.erase(bBuyOrders.begin());
			bool mmSell = false;
			if (mmbBuyOrders.size() == 0) {
				mmSell = false;
			} else {
				int u = 0;
				bool found = false;
				while (!found && u<bBuyOrders.size()) {
					if (mmbBuyOrders[0] == bBuyOrders[u]) {
						found = true;
						/*
						cout << endl << "orders: " << mmbSellOrders[0].GetPrice() << endl;
						cout << endl << "ITS IN THERE" << endl;
						for (int i = 0; i<bSellOrders.size(); i++) {
							cout << bSellOrders[i].GetPrice() << " ";
						}
						*/
						
					} else {
						u++;
					}
				}
				if (!found) {
					mmSell = true;
				}
			}
			//output transaction to ticker
			if (mmSell) {
				cout << " (MM buy)";
				boughtbOrders++;
				totalMMcash -= bBuyQuote;
				mmbBuyOrders.clear();
				currentbBuyOrder = Order("B", 1, 0);
			}
			cout << endl;

			cout << "F: " << fBuyQuote;
			fPrice = fBuyQuote;
			fBuyOrders.erase(fBuyOrders.begin());
			mmSell = false;
			if (mmfBuyOrders.size() == 0) {
				mmSell = false;
			} else {
				int u = 0;
				bool found = false;
				while (!found && u<fBuyOrders.size()) {
					if (mmfBuyOrders[0] == fBuyOrders[u]) {
						found = true;
					} else {
						u++;
					}
				}
				if (!found) {
					mmSell = true;
				}
			}
			//output transaction to ticker
			if (mmSell) {
				cout << " (MM buy)";
				boughtfOrders++;
				totalMMcash -= fBuyQuote;
				mmfBuyOrders.clear();
				currentfBuyOrder = Order("F", 1, 0);
			}
			cout << endl;


			cout << "I: " << iSellQuote;
			iPrice = iSellQuote;
			iSellOrders.erase(iSellOrders.begin());
			mmSell = false;
			if (mmiBuyOrders.size() == 0) {
				mmSell = false;
			} else {
				int u = 0;
				bool found = false;
				while (!found && u<iBuyOrders.size()) {
					if (mmiSellOrders[0] == iSellOrders[u]) {
						found = true;
					} else {
						u++;
					}
				}
				if (!found) {
					mmSell = true;
				}
			}
			//output transaction to ticker
			if (mmSell) {
				cout << " (MM sell)";
				soldiOrders++;
				totalMMcash += iSellQuote;
				mmiSellOrders.clear();
				currentiSellOrder = Order("I", 1, 0);
			}
			cout << endl;


			cout << "Index Arbitrage: profit = " << iaProfit << endl;
			
		}
		
		//task 3 do MM: 
		
		if (bSellQuote-bBuyQuote >= mmSpreadThreshold && bBuyQuote != 0 && 
			bSellQuote != 0 && bBuyOrders.size() > 0 && bSellOrders.size() > 0 && 
			bBuyOrders.at(0).GetPrice() == bBuyQuote && 
			bSellOrders.at(0).GetPrice() == bSellQuote) {
			//if no current order for buying B
			if (currentbBuyOrder.GetPrice() == 0) {
				//create order at one price above bid quote
				currentbBuyOrder = Order("B", 1, bBuyQuote+1);
				
				//insert MM's B buy order (one above current bid quote) 
				//  into sorted vector.
				bool foundHigher = false;
				int j = 0;
				/*
				while (!foundHigher && j<bBuyOrders.size()) {
					if (currentbBuyOrder.GetPrice() > bBuyOrders.at(j).GetPrice()) {
						foundHigher = true;
					} else {
						j++;
					}
				}
				if (j == bBuyOrders.size() && !foundHigher) {
					bBuyOrders.push_back(currentbBuyOrder);
				} else {
					bBuyOrders.insert(bBuyOrders.begin()+j, currentbBuyOrder);
				}
				*/
				bBuyOrders.insert(bBuyOrders.begin(), currentbBuyOrder);
				//add put-out order to MM's orders
				mmbBuyOrders.push_back(currentbBuyOrder);
			//if has current buy order below bid quote, w/draw current 
			//  quote, replace w/ buy order @ one above bid quote
			} else if (currentbBuyOrder.GetPrice() < bBuyQuote) {
				mmbBuyOrders.clear();
				currentbBuyOrder = Order("B", 1, 0);
				bool found = false;
				int p = 0;
				// w/draw buy order in vector of current buy orders out
				while (!found && p<bBuyOrders.size()) {
					if (bBuyOrders[p] == currentbBuyOrder) {
						bBuyOrders.erase(bBuyOrders.begin() + p);
						found = true;
					} else {
						p++;
					}
				}
				currentbBuyOrder = Order("B", 1, bBuyQuote+1);
				mmbBuyOrders.push_back(currentbBuyOrder);
				//insert current MM B buy order into vector of all b buy orders
				/*
				bool foundHigher = false;
				int j = 0;
				while (!foundHigher && j<bBuyOrders.size()) {
					if (currentbBuyOrder.GetPrice() > bBuyOrders.at(j).GetPrice()) {
						foundHigher = true;
					} else {
						j++;
					}
				}
				if (j == bBuyOrders.size() && !foundHigher) {
					bBuyOrders.push_back(currentbBuyOrder);
				} else {
					bBuyOrders.insert(bBuyOrders.begin()+j, currentbBuyOrder);
				}
				*/
				bBuyOrders.insert(bBuyOrders.begin(), currentbBuyOrder);
			}



			if (currentbSellOrder.GetPrice() == 0) {
				//create order at one price below ask quote
				currentbSellOrder = Order("B", 1, bSellQuote-1);
				
				//insert MM's B sell order (one below current ask quote) 
				//  into sorted vector.
				/*
				bool foundHigher = false;
				int j = 0;
				while (!foundHigher && j<bSellOrders.size()) {
					if (currentbSellOrder.GetPrice() < bSellOrders.at(j).GetPrice()) {
						foundHigher = true;
					} else {
						j++;
					}
				}
				//current order has highest price
				if (j == bSellOrders.size()) {
					bSellOrders.push_back(currentbSellOrder);
				} else {
					bSellOrders.insert(bSellOrders.begin()+j, currentbSellOrder);
				}
				*/
				bSellOrders.insert(bSellOrders.begin(), currentbSellOrder);
				//add put-out order to MM's orders
				mmbSellOrders.push_back(currentbSellOrder);
			//if has current sell order above ask quote, w/draw current 
			//  quote, replace w/ sell order @ one below ask quote
			} else if (currentbSellOrder.GetPrice() > bSellQuote) {
				mmbSellOrders.clear();
				currentbSellOrder = Order("B", -1, 0);
				bool found = false;
				int p = 0;
				// w/draw sell order in vector of current sell orders out
				while (!found && p<bSellOrders.size()) {
					if (bSellOrders[p] == currentbSellOrder) {
						bSellOrders.erase(bSellOrders.begin() + p);
						found = true;
					} else {
						p++;
					}
				}
				currentbSellOrder = Order("B", 1, bSellQuote-1);
				mmbSellOrders.push_back(currentbSellOrder);
				//insert current MM B sell order into vector of all b sell orders
				/*
				bool foundHigher = false;
				int j = 0;
				while (!foundHigher && j<bSellOrders.size()) {
					if (currentbSellOrder.GetPrice() > bSellOrders.at(j).GetPrice()) {
						foundHigher = true;
					} else {
						j++;
					}
				}
				if (j == bSellOrders.size() && !foundHigher) {
					bSellOrders.push_back(currentbSellOrder);
				} else {
					bSellOrders.insert(bSellOrders.begin()+j, currentbSellOrder);
				}
				*/
				bSellOrders.insert(bSellOrders.begin(), currentbSellOrder);

			}
		}

		if (fSellQuote-fBuyQuote >= mmSpreadThreshold && fBuyQuote != 0 && 
			fSellQuote != 0 && fBuyOrders.size() > 0 && fSellOrders.size() > 0 
			&& fBuyOrders.at(0).GetPrice() == fBuyQuote && 
			fSellOrders.at(0).GetPrice() == fSellQuote) {
			//if no current order for buying F
			//I, -1, 86
			if (currentfBuyOrder.GetPrice() == 0) {
				//create order at one price above bid quote
				currentfBuyOrder = Order("F", 1, fBuyQuote+1);
				
				//insert MM's F buy order (one above current bid quote) 
				//  into sorted vector.
				/*
				bool foundHigher = false;
				int j = 0;
				while (!foundHigher && j<fBuyOrders.size()) {
					if (currentfBuyOrder.GetPrice() > fBuyOrders.at(j).GetPrice()) {
						foundHigher = true;
					} else {
						j++;
					}
				}
				if (j == fBuyOrders.size() && !foundHigher) {
					fBuyOrders.push_back(currentfBuyOrder);
				} else {
					fBuyOrders.insert(fBuyOrders.begin()+j, currentfBuyOrder);
				}
				*/
				fBuyOrders.insert(fBuyOrders.begin(), currentfBuyOrder);
				//add put-out order to MM's orders
				mmfBuyOrders.push_back(currentfBuyOrder);
			//if has current buy order below bid quote, w/draw current 
			//  quote, replace w/ buy order @ one above bid quote
			} else if (currentfBuyOrder.GetPrice() < fBuyQuote) {
				mmfBuyOrders.clear();
				currentfBuyOrder = Order("F", 1, 0);
				bool found = false;
				int p = 0;
				// w/draw buy order in vector of current buy orders out
				while (!found && p<fBuyOrders.size()) {
					if (fBuyOrders[p] == currentfBuyOrder) {
						fBuyOrders.erase(fBuyOrders.begin() + p);
						found = true;
					} else {
						p++;
					}
				}
				currentfBuyOrder = Order("F", 1, fBuyQuote+1);
				mmfBuyOrders.push_back(currentfBuyOrder);

				fBuyOrders.insert(fBuyOrders.begin(), currentfBuyOrder);
				//insert current MM F buy order into vector of all F buy orders
				/*
				bool foundHigher = false;
				int j = 0;
				while (!foundHigher && j<fBuyOrders.size()) {
					if (currentfBuyOrder.GetPrice() > fBuyOrders.at(j).GetPrice()) {
						foundHigher = true;
					} else {
						j++;
					}
				}
				if (j == fBuyOrders.size() && !foundHigher) {
					fBuyOrders.push_back(currentfBuyOrder);
				} else {
					fBuyOrders.insert(fBuyOrders.begin()+j, currentfBuyOrder);
				}
				*/
			}



			if (currentfSellOrder.GetPrice() == 0) {
				//create order at one price below ask quote
				currentfSellOrder = Order("F", 1, fSellQuote-1);
				
				//insert MM's F sell order (one below current ask quote) 
				//  into sorted vector.
				/*
				bool foundHigher = false;
				int j = 0;
				while (!foundHigher && j<fSellOrders.size()) {
					if (currentfSellOrder.GetPrice() > fSellOrders.at(j).GetPrice()) {
						foundHigher = true;
					} else {
						j++;
					}
				}
				if (j == fSellOrders.size() && !foundHigher) {
					fSellOrders.push_back(currentfSellOrder);
				} else {
					fSellOrders.insert(fSellOrders.begin()+j, currentfSellOrder);
				}
				*/
				fSellOrders.insert(fSellOrders.begin(), currentfSellOrder);
				//add put-out order to MM's orders
				mmfSellOrders.push_back(currentfSellOrder);
			//if has current sell order above ask quote, w/draw current 
			//  quote, replace w/ sell order @ one below ask quote
			} else if (currentfSellOrder.GetPrice() > fSellQuote) {
				mmfSellOrders.clear();
				currentfSellOrder = Order("F", -1, 0);
				bool found = false;
				int p = 0;
				// w/draw sell order in vector of current sell orders out
				while (!found && p<fSellOrders.size()) {
					if (fSellOrders[p] == currentfSellOrder) {
						fSellOrders.erase(fSellOrders.begin() + p);
						found = true;
					} else {
						p++;
					}
				}
				currentfSellOrder = Order("F", 1, fSellQuote-1);
				mmfSellOrders.push_back(currentfSellOrder);
				//insert current MM F sell order into vector of all F sell orders
				/*
				bool foundHigher = false;
				int j = 0;
				while (!foundHigher && j<fSellOrders.size()) {
					if (currentfSellOrder.GetPrice() > fSellOrders.at(j).GetPrice()) {
						foundHigher = true;
					} else {
						j++;
					}
				}
				if (j == fSellOrders.size() && !foundHigher) {
					fSellOrders.push_back(currentfSellOrder);
				} else {
					fSellOrders.insert(fSellOrders.begin()+j, currentfSellOrder);
				}
				*/
				fSellOrders.insert(fSellOrders.begin(), currentfSellOrder);

			}
		}

		if (iSellQuote-iBuyQuote >= mmSpreadThreshold 
			&& iBuyQuote != 0 && iSellQuote != 0 
			&& iBuyOrders.size() > 0 && iSellOrders.size() > 0 
			&& iBuyOrders.at(0).GetPrice() == iBuyQuote 
			&& iSellOrders.at(0).GetPrice() == iSellQuote) {
			//if no current order for buying I
			if (currentiBuyOrder.GetPrice() == 0) {
				//create order at one price above bid quote
				currentiBuyOrder = Order("I", 1, iBuyQuote+1);
				
				//insert MM's I buy order (one above current bid quote) 
				//  into sorted vector.
				/*
				bool foundHigher = false;
				int j = 0;
				while (!foundHigher && j<iBuyOrders.size()) {
					if (currentiBuyOrder.GetPrice() > iBuyOrders.at(j).GetPrice()) {
						foundHigher = true;
					} else {
						j++;
					}
				}
				if (j == iBuyOrders.size() && !foundHigher) {
					iBuyOrders.push_back(currentiBuyOrder);
				} else {
					iBuyOrders.insert(iBuyOrders.begin()+j, currentiBuyOrder);
				}
				*/
				iBuyOrders.insert(iBuyOrders.begin(), currentiBuyOrder);
				//add put-out order to MM's orders
				mmiBuyOrders.push_back(currentiBuyOrder);
			//if has current buy order below bid quote, w/draw current 
			//  quote, replace w/ buy order @ one above bid quote
			} else if (currentiBuyOrder.GetPrice() < iBuyQuote) {
				mmiBuyOrders.clear();
				currentiBuyOrder = Order("I", 1, 0);
				bool found = false;
				int p = 0;
				// w/draw buy order in vector of current buy orders out
				while (!found && p<iBuyOrders.size()) {
					if (iBuyOrders[p] == currentiBuyOrder) {
						iBuyOrders.erase(iBuyOrders.begin() + p);
						found = true;
					} else {
						p++;
					}
				}
				currentiBuyOrder = Order("I", 1, iBuyQuote+1);
				mmiBuyOrders.push_back(currentiBuyOrder);
				//insert current MM B buy order into vector of all b buy orders
				/*
				bool foundHigher = false;
				int j = 0;
				while (!foundHigher && j<iBuyOrders.size()) {
					if (currentiBuyOrder.GetPrice() > iBuyOrders.at(j).GetPrice()) {
						foundHigher = true;
					} else {
						j++;
					}
				}
				if (j == iBuyOrders.size() && !foundHigher) {
					iBuyOrders.push_back(currentiBuyOrder);
				} else {
					iBuyOrders.insert(iBuyOrders.begin()+j, currentiBuyOrder);
				}
				*/
				iBuyOrders.insert(iBuyOrders.begin(), currentiBuyOrder);
			}


			if (currentiSellOrder.GetPrice() == 0) {
				//create order at one price below ask quote
				currentiSellOrder = Order("I", 1, iSellQuote-1);
				
				//insert MM's I sell order (one below current ask quote) 
				//  into sorted vector.
				/*
				bool foundHigher = false;
				int j = 0;
				while (!foundHigher && j<iSellOrders.size()) {
					if (currentiSellOrder.GetPrice() < iSellOrders.at(j).GetPrice()) {
						foundHigher = true;
					} else {
						j++;
					}
				}
				//current order has highest price
				if (j == iSellOrders.size()) {
					iSellOrders.push_back(currentiSellOrder);
				} else {
					iSellOrders.insert(iSellOrders.begin()+j, currentiSellOrder);
				}
				*/
				iSellOrders.insert(iSellOrders.begin(), currentiSellOrder);
				//add put-out order to MM's orders
				mmiSellOrders.push_back(currentiSellOrder);
			//if has current sell order above ask quote, w/draw current 
			//  quote, replace w/ sell order @ one below ask quote
			} else if (currentiSellOrder.GetPrice() > iSellQuote) {
				mmiSellOrders.clear();
				currentiSellOrder = Order("I", -1, 0);
				bool found = false;
				int p = 0;
				// w/draw sell order in vector of current sell orders out
				while (!found && p<iSellOrders.size()) {
					if (iSellOrders[p] == currentiSellOrder) {
						iSellOrders.erase(iSellOrders.begin() + p);
						found = true;
					} else {
						p++;
					}
				}
				currentiSellOrder = Order("I", 1, iSellQuote-1);
				mmiSellOrders.push_back(currentiSellOrder);
				//insert current MM I sell order into vector of all I sell orders
				/*
				bool foundHigher = false;
				int j = 0;
				while (!foundHigher && j<iSellOrders.size()) {
					if (currentiSellOrder.GetPrice() < iSellOrders.at(j).GetPrice()) {
						foundHigher = true;
					} else {
						j++;
					}
				}
				//current order has highest price
				if (j == iSellOrders.size()) {
					iSellOrders.push_back(currentiSellOrder);
				} else {
					iSellOrders.insert(iSellOrders.begin()+j, currentiSellOrder);
				}
				*/
				iSellOrders.insert(iSellOrders.begin(), currentiSellOrder);

			}
		}

		/*
		cout << "current B MM Buy Orders: ";
		for (int i = 0; i<mmbBuyOrders.size(); i++) {
			cout << mmbBuyOrders[i].GetPrice() << " ";
		}
		cout << endl;
		cout << "current B MM Sell Orders: ";
		for (int i = 0; i<mmbSellOrders.size(); i++) {
			cout << mmbSellOrders[i].GetPrice() << " ";
		}
		cout << endl;
		cout << "current F MM Buy Orders: ";
		for (int i = 0; i<mmfBuyOrders.size(); i++) {
			cout << mmfBuyOrders[i].GetPrice() << " ";
		}
		cout << endl;
		cout << "current F MM Sell Orders: ";
		for (int i = 0; i<mmfSellOrders.size(); i++) {
			cout << mmfSellOrders[i].GetPrice() << " ";
		}
		cout << endl;
		cout << "current I MM Buy Orders: ";
		for (int i = 0; i<mmiBuyOrders.size(); i++) {
			cout << mmiBuyOrders[i].GetPrice() << " ";
		}
		cout << endl;
		cout << "current I MM Sell Orders: ";
		for (int i = 0; i<mmiSellOrders.size(); i++) {
			cout << mmiSellOrders[i].GetPrice() << " ";
		}
		cout << endl << endl;
		*/

		orderFS >> currentSymbol;
	}

	cout << "IA final holdings: cash = " << totaliaProfit << endl;
	cout << "MM final holdings: cash = " << totalMMcash << endl;
	int bUnits = boughtbOrders - soldbOrders;
	cout << "Security B: " << boughtbOrders - soldbOrders << 
	" units, value = " << bUnits * bPrice << endl;
	int fUnits = boughtfOrders - soldfOrders;
	cout << "Security F: " << fUnits << " units, value = " 
	<< fUnits * fPrice << endl;
	int iUnits = boughtiOrders - soldiOrders;
	cout << "Security I: " << iUnits << " units, value = " 
	<< iUnits * iPrice << endl;
	cout << "Total profit is " << totalMMcash + (bUnits*bPrice) + 
	(fUnits*fPrice) + (iUnits*iPrice) << "." << endl;


}
