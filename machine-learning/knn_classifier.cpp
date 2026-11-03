// k-Nearest Neighbours classifier implemented by hand
// Toy dataset: classify fruit as Apple / Orange / Banana from (weight in grams, length in cm)
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

struct Sample {
    double weight, length;
    string label;
};

double distance(const Sample& a, const Sample& b) {
    // features have different scales, so weight is divided by 10 before comparing
    double dw = (a.weight - b.weight) / 10.0, dl = a.length - b.length;
    return sqrt(dw * dw + dl * dl);
}

string predict(const vector<Sample>& train, const Sample& q, int k, int skipIndex = -1) {
    vector<pair<double, string>> d;
    for (size_t i = 0; i < train.size(); i++)
        if (static_cast<int>(i) != skipIndex) d.push_back({distance(train[i], q), train[i].label});
    sort(d.begin(), d.end());
    map<string, int> votes;
    for (int i = 0; i < k && i < static_cast<int>(d.size()); i++) votes[d[i].second]++;
    string best;
    int bestVotes = -1;
    for (auto& [label, v] : votes)
        if (v > bestVotes) { best = label; bestVotes = v; }
    return best;
}

int main() {
    vector<Sample> data = {
        {150, 7.5, "Apple"},  {170, 8.0, "Apple"},  {140, 7.0, "Apple"},  {160, 7.8, "Apple"},  {180, 8.2, "Apple"},
        {130, 6.5, "Orange"}, {120, 6.2, "Orange"}, {145, 6.8, "Orange"}, {125, 6.0, "Orange"}, {135, 6.4, "Orange"},
        {120, 18.0, "Banana"}, {115, 17.0, "Banana"}, {130, 19.5, "Banana"}, {110, 16.5, "Banana"}, {125, 18.5, "Banana"},
    };
    cout << fixed << setprecision(1);
    cout << "Training set: " << data.size() << " fruits (5 per class)\n\n";

    cout << "Leave-one-out accuracy (each sample is predicted from the other " << data.size() - 1 << "):\n";
    for (int k : {1, 3, 5, 7}) {
        int correct = 0;
        for (size_t i = 0; i < data.size(); i++)
            if (predict(data, data[i], k, static_cast<int>(i)) == data[i].label) correct++;
        cout << "  k = " << k << "  ->  " << correct << "/" << data.size() << " correct ("
             << 100.0 * correct / data.size() << "%)\n";
    }

    cout << "\nNew fruits (k = 3):\n";
    vector<Sample> unknown = {{165, 7.9, "?"}, {128, 6.3, "?"}, {118, 17.8, "?"}, {142, 7.1, "?"}};
    for (const Sample& u : unknown)
        cout << "  weight " << u.weight << " g, length " << u.length << " cm  ->  " << predict(data, u, 3) << '\n';
    return 0;
}
