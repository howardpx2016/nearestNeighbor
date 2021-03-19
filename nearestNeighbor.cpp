#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>
using namespace std;

double findDistance(vector<double> object_to_classify, vector<double> neighbor_to_classify, vector<int> set_of_features, int k) {
    double sum = 0;
    double distance = 0;
    // create new set to include k
    vector<int> features_plus_k = set_of_features;
    features_plus_k.push_back(k);

    //test: check features tested
    cout << "--accounting for feature ";
    
    //calculate sum
    for (int i = 1; i < object_to_classify.size(); i++) {
        // account for feature i if i is found in features_plus_k
        if (find(features_plus_k.begin(), features_plus_k.end(), i) != features_plus_k.end()) {
            cout << i << ", ";
            double difference = neighbor_to_classify.at(i) - object_to_classify.at(i);
            sum += pow(difference, 2);
        }
    }
    cout << endl;
    distance = sqrt(sum);
    return distance;
}

double leave_one_out_cross_validation(vector<vector<double>> data, vector<int> set_of_features, int k) {
    int number_correctly_classfied = 0; // keep track of correct classifications
    //iterate through each object
    for (int i = 0; i < data.size(); i++) {
        vector<double> object_to_classify = data.at(i); // current object
        double label_object_to_classify = object_to_classify.at(0); // object class
        int nearest_neighbor = 0; // keep track of nearest neighbor
        double label_neighbor_to_classify = 0; // keep track of nearest neighbor's class
        double inf = numeric_limits<double>::infinity();
        double nearest_neighbor_distance = inf; // keep track of nearest neighbor's distance
        
        // test: check if class labels are correct
        // cout << "--Object " << i+1 << " is in class " << label_object_to_classify << endl;
        
        //compare object with each of its neighbors
        for (int n = 0; n < data.size(); n++) {
            if (n != i) {
                // cout << "--Ask if " << i+1 << " is the nearest neighbor with " << n+1 << endl;
                vector<double> neighbor_to_classify = data.at(n);
                double distance = findDistance(object_to_classify, neighbor_to_classify, set_of_features, k); // get distance between object and neighbor
                
                //test: show distance of current neighbor
                // cout << "--Distance between object " << i+1 << " and " << n+1 << " is " << distance << endl;
                
                //update nearest neighbor, its distance, and class 
                if (distance < nearest_neighbor_distance) {
                    nearest_neighbor_distance = distance; //                     nearest_neighbor_distance = distance;
                    nearest_neighbor = n+1;//                     nearest_neighbor_location = k;
                    label_neighbor_to_classify = neighbor_to_classify.at(0); //                     nearest_neighbor_label    = data(nearest_neighbor_location,1);
                }
            }
        }
        
        // cout << "--Its nearest neighbor is " << nearest_neighbor << " which is class " << label_neighbor_to_classify << endl;
        // update number of correct answers
        if (label_object_to_classify == label_neighbor_to_classify) {
            number_correctly_classfied++;
            //test: print if correct
            // cout << "--CORRECT" << endl;
        }
        //test: print if wrong
        // else {
        //     cout << "--WRONG" << endl;
        // }
        
    } 
    //test: print accuracy
    double accuracy = double(number_correctly_classfied) / double(data.size());
    cout << "--" << number_correctly_classfied << " correct, " << data.size() - number_correctly_classfied << " wrong. Accuracy = " 
        << accuracy*100 << "%" << endl;

    return accuracy;
}

void featureSearch(vector<vector<double>> data) {
    vector<int> current_set_of_features; // start with an empty set of features
    
    for (int i = 1; i < data.size(); i++) {
        cout << "On level " << i << " of the search tree" << endl;
        int feature_to_add_at_this_level = 0; // keep track of feature to add
        double best_so_far_accuracy = 0.0; // keep track of best accuracy  
        
        for (int k = 1; k < data.size(); k++) {
            // check accuracy of k if k doesn't already exist
            if (find(current_set_of_features.begin(), current_set_of_features.end(), k) == current_set_of_features.end()) { 
                cout << "--Considering adding feature " << k << endl;
                double accuracy = leave_one_out_cross_validation(data, current_set_of_features, k); // get accuracy after adding k to set
                // update highest accuracy (from feature k) if applicable
                if (accuracy > best_so_far_accuracy) {
                    best_so_far_accuracy = accuracy;
                    feature_to_add_at_this_level = k;            
                }      
            }
        }
        
        current_set_of_features.push_back(feature_to_add_at_this_level);
        cout << "On level " << i  << ", I added feature " << feature_to_add_at_this_level << " to current set\n" << endl;
    }
       
}

int main() {
    // save input data into 2d vector 'data'
    vector<vector<double>> data;
    // ifstream fin("CS170_SMALLtestdata__6.txt");
    ifstream fin("testIn.txt");
    if (fin.is_open()) {
        double element;
        while (fin.good()) {
            vector<double> temp;
            double element;
            string input;
            getline(fin, input);
            stringstream sstream;
            sstream << input;
            while (sstream >> element) {
                temp.push_back(element);
            }
            data.push_back(temp);
        }
    }
    else {
        cout << "unable to open input file" << endl;
        return 1;
    }
    fin.close();

    featureSearch(data);
    //test accuracy
    // vector<int> featuresetTest = {};
    // double k = leave_one_out_cross_validation(data, featuresetTest, 2);

    //test: output data into file
    double currValue;
    ofstream fout("testOut.txt");
    if (fout.is_open()) {
        for (int i = 0; i < data.size(); i++) {
            for (int j = 0; j < data.at(i).size(); j++) {
                fout << data[i][j] << "  ";
            }
            if (i < data.size() - 1) {
                fout << endl;
            }
        }
    }
    else {
        cout << "unable to open output file" << endl;
        return 1;
    }
    fout.close();

    return 0;
}