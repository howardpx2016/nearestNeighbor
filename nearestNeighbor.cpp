#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>
#include "get_time.h"
using namespace std;

void printFeatureSet(vector<int> &v) {
    cout << "{";
    for (int i = 0; i < v.size(); i++) {
        if (i+1 < v.size()) {
            cout << v.at(i) << ", ";
        }
        else {
            cout << v.at(i);
        }
    }
    cout << "}";
}

void remove_k(vector<int> &v, int k) {
    vector<int>::iterator it = find(v.begin(), v.end(), k);
    v.erase(it);
    // printFeatureSet(v);
    // cout << endl;
}

double findDistance(vector<double> &object_to_classify, vector<double> &neighbor_to_classify, vector<int> &updated_feature_set) {
    double sum = 0;
    double distance = 0;
    //calculate sum
    for (int i = 1; i < object_to_classify.size(); i++) {
        // account for feature i if i is found in updated_feature_set
        if (find(updated_feature_set.begin(), updated_feature_set.end(), i) != updated_feature_set.end()) {
            // cout << i << ", ";
            double difference = neighbor_to_classify.at(i) - object_to_classify.at(i);
            sum += pow(difference, 2);
        }
    }
    // cout << endl;
    distance = sqrt(sum);
    return distance;
}

double leave_one_out_cross_validation(vector<vector<double>> &data, vector<int> &set_of_features, int k, bool isForward) {
    int number_correctly_classfied = 0; // keep track of correct classifications
    // create new set to include k
    vector<int> updated_feature_set = set_of_features;
    if (isForward) {
        updated_feature_set.push_back(k);
    }
    else {
        remove_k(updated_feature_set, k);
    }
    // print updated_feature_set
    cout << "       Using features(s) ";
    printFeatureSet(updated_feature_set);

    //iterate through each object
    for (int i = 0; i < data.size(); i++) {
        vector<double> object_to_classify = data.at(i); // current object
        double label_object_to_classify = object_to_classify.at(0); // object class
        int nearest_neighbor = 0; // keep track of nearest neighbor
        double label_neighbor_to_classify = 0; // keep track of nearest neighbor's class
        double inf = numeric_limits<double>::infinity();
        double nearest_neighbor_distance = inf; // keep track of nearest neighbor's distance

        //compare object with each of its neighbors
        for (int n = 0; n < data.size(); n++) {
            if (n != i) {
                // cout << "--Ask if " << i+1 << " is the nearest neighbor with " << n+1 << endl;
                vector<double> neighbor_to_classify = data.at(n);

                double distance = findDistance(object_to_classify, neighbor_to_classify, updated_feature_set); // get distance between object and neighbor
                
                //update nearest neighbor, its distance, and class 
                if (distance < nearest_neighbor_distance) {
                    nearest_neighbor_distance = distance;           
                    nearest_neighbor = n+1;       
                    label_neighbor_to_classify = neighbor_to_classify.at(0); 
                }
            }
        }

        // update number of correct answers
        if (label_object_to_classify == label_neighbor_to_classify) {
            number_correctly_classfied++;
        }
        
    } 
    // print accuracy
    double accuracy = double(number_correctly_classfied) / double(data.size());
    cout << " accuracy is " << accuracy*100 << "%" << endl;

    return accuracy;
}

void forwardFeatureSearch(vector<vector<double>> &data) {
    vector<int> current_set_of_features; // start with an empty set of features
    vector<int> best_feature_subset; // keep track of most accurate feature set
    double overall_best_accuracy = 0.0;
    cout << "Begin Search" << endl;
    
    //iterate through each item
    for (int i = 1; i < data.at(0).size(); i++) {
        cout << "On level " << i << " of the search tree" << endl;
        int feature_to_add_at_this_level = 0; // keep track of feature to add
        double level_best_accuracy = 0.0; // keep track of best accuracy 

        // iterate through features
        for (int k = 1; k < data.at(0).size(); k++) {
            // check accuracy of curr set + k if k doesn't already exist in current featureset
            if (find(current_set_of_features.begin(), current_set_of_features.end(), k) == current_set_of_features.end()) { 
                // get accuracy after adding k to current set
                double accuracy = leave_one_out_cross_validation(data, current_set_of_features, k, 1); 
                // update highest accuracy (from feature k) if applicable
                if (accuracy > level_best_accuracy) {
                    level_best_accuracy = accuracy;
                    feature_to_add_at_this_level = k;            
                }      
            }
        }
        
        current_set_of_features.push_back(feature_to_add_at_this_level); // update curr feature set
        if (level_best_accuracy > overall_best_accuracy) { // update best feature set and accuracy if current set is better
            overall_best_accuracy = level_best_accuracy;
            best_feature_subset = current_set_of_features;  
        }

        cout << "Feature set ";
        printFeatureSet(current_set_of_features);
        cout << " was best, accuracy is " << level_best_accuracy*100 << "%" << endl;
    }
    cout << "\nFinished Search! The best feature subset so is ";
    printFeatureSet(best_feature_subset);
    cout << " with an accuracy of " << overall_best_accuracy*100 << "%" << endl;
}

void backwardFeatureSearch(vector<vector<double>> &data) {
    vector<int> current_set_of_features; // start with an empty set of features
    for (int h = 1; h < data.at(0).size(); h++) {
        current_set_of_features.push_back(h);
    }
    // printFeatureSet(current_set_of_features);
    vector<int> best_feature_subset; // keep track of most accurate feature set
    double overall_best_accuracy = 0.0;
    cout << "Begin Search" << endl;
    
    //iterate through each item
    for (int i = 1; i < data.at(0).size(); i++) {
        cout << "On level " << i << " of the search tree" << endl;
        int feature_to_remove_at_this_level = 0; // keep track of feature to remove
        double level_best_accuracy = 0.0; // keep track of best accuracy 

        // iterate through features
        for (int k = 1; k < data.at(0).size(); k++) {
            // check accuracy of curr set without k if k hasn't been removed from current featureset
            if (find(current_set_of_features.begin(), current_set_of_features.end(), k) != current_set_of_features.end()) { 
                // get accuracy after removing k from current set
                double accuracy = leave_one_out_cross_validation(data, current_set_of_features, k, 0); 
                // update highest accuracy (from feature k) if applicable
                if (accuracy > level_best_accuracy) {
                    level_best_accuracy = accuracy;
                    feature_to_remove_at_this_level = k;            
                }      
            }
        }
        
        remove_k(current_set_of_features, feature_to_remove_at_this_level); // update curr feature set
        if (level_best_accuracy > overall_best_accuracy) { // update best feature set and accuracy if current set is better
            overall_best_accuracy = level_best_accuracy;
            best_feature_subset = current_set_of_features;  
        }

        cout << "Feature set ";
        printFeatureSet(current_set_of_features);
        cout << " was best, accuracy is " << level_best_accuracy*100 << "%" << endl;
    }
    cout << "\nFinished Search!The best feature subset so is ";
    printFeatureSet(best_feature_subset);
    cout << " with an accuracy of " << overall_best_accuracy*100 << "%" << endl;
}

int main() {
    string fileName;
    int algChoice;

    cout << "Welcome to the Feature Selection Algorithm." << endl;
    cout << "Type in the name of the file to test : ";
        cin >> fileName;
    cout << "Type the number of the algorithm you want to run." << endl;
        cout << "   1) Forward Selection" << endl;
        cout << "   2) Backward Elimination" << endl;
        cout << "   Your algorithm of choice: ";
        cin >> algChoice;
        cout << endl;

    // save input data into 2d vector 'data'
    vector<vector<double>> data;
    // ifstream fin("CS170_SMALLtestdata__6.txt");
    ifstream fin(fileName);
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
        data.pop_back();
    }
    else {
        cout << "unable to open input file" << endl;
        return 1;
    }
    fin.close();

    if (algChoice == 1) {
        timer s;
        forwardFeatureSearch(data);
        s.stop();
        cout << "Time elapsed: " << s.get_total() << endl;
        cout << endl;
    }
    else if (algChoice == 2) {
        timer t;
        backwardFeatureSearch(data);
        t.stop();
        cout << "Time elapsed: " << t.get_total() << endl;
    }
    else {
        cout << "Algorithm Choice doesn't exist" << endl;
        return 1;
    }

    return 0;
}