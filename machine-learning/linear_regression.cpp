// Simple linear regression implemented by hand (no libraries)
// Dataset: hours studied per week -> exam score (small made-up dataset for practice)
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

struct Model { double slope, intercept; };

double mean(const vector<double>& v) {
    double s = 0;
    for (double x : v) s += x;
    return s / v.size();
}

// 1) closed-form least squares:  m = sum((x-mx)(y-my)) / sum((x-mx)^2)
Model leastSquares(const vector<double>& x, const vector<double>& y) {
    double mx = mean(x), my = mean(y), num = 0, den = 0;
    for (size_t i = 0; i < x.size(); i++) {
        num += (x[i] - mx) * (y[i] - my);
        den += (x[i] - mx) * (x[i] - mx);
    }
    double m = num / den;
    return {m, my - m * mx};
}

double mse(const Model& md, const vector<double>& x, const vector<double>& y) {
    double s = 0;
    for (size_t i = 0; i < x.size(); i++) {
        double e = (md.slope * x[i] + md.intercept) - y[i];
        s += e * e;
    }
    return s / x.size();
}

// 2) gradient descent on the mean squared error
Model gradientDescent(const vector<double>& x, const vector<double>& y, double lr, int epochs) {
    Model md{0, 0};
    const double n = x.size();
    for (int e = 1; e <= epochs; e++) {
        double gm = 0, gb = 0;
        for (size_t i = 0; i < x.size(); i++) {
            double err = (md.slope * x[i] + md.intercept) - y[i];
            gm += (2 / n) * err * x[i];
            gb += (2 / n) * err;
        }
        md.slope -= lr * gm;
        md.intercept -= lr * gb;
        if (e == 1 || e % 2000 == 0)
            cout << "  epoch " << setw(5) << e << ": slope=" << setw(7) << md.slope << " intercept=" << setw(7)
                 << md.intercept << " MSE=" << mse(md, x, y) << '\n';
    }
    return md;
}

double rSquared(const Model& md, const vector<double>& x, const vector<double>& y) {
    double my = mean(y), ssRes = 0, ssTot = 0;
    for (size_t i = 0; i < x.size(); i++) {
        double p = md.slope * x[i] + md.intercept;
        ssRes += (y[i] - p) * (y[i] - p);
        ssTot += (y[i] - my) * (y[i] - my);
    }
    return 1 - ssRes / ssTot;
}

int main() {
    vector<double> hours = {2, 3, 4, 5, 6, 7, 8, 9, 10, 12};
    vector<double> score = {48, 52, 57, 61, 64, 70, 73, 79, 82, 90};
    cout << "Dataset (hours -> score):\n  ";
    for (size_t i = 0; i < hours.size(); i++) cout << hours[i] << "->" << score[i] << "  ";
    cout << fixed << setprecision(3);
    cout << "\n\n1) Least squares (closed form)\n";
    Model ls = leastSquares(hours, score);
    cout << "  score = " << ls.slope << " * hours + " << ls.intercept << '\n';
    cout << "  MSE = " << mse(ls, hours, score) << ", R^2 = " << rSquared(ls, hours, score) << "\n\n";

    cout << "2) Gradient descent (learning rate 0.01, 10000 epochs)\n";
    Model gd = gradientDescent(hours, score, 0.01, 10000);
    cout << "  score = " << gd.slope << " * hours + " << gd.intercept << '\n';
    cout << "  difference from least squares: slope " << fabs(gd.slope - ls.slope) << ", intercept "
         << fabs(gd.intercept - ls.intercept) << "\n\n";

    cout << "Predictions:\n";
    for (double h : {1.0, 6.5, 11.0, 15.0})
        cout << "  " << setw(5) << h << " hours -> predicted score " << ls.slope * h + ls.intercept << '\n';
    cout << "Note: 15 hours is outside the data range. The model predicts more than 100,\n"
         << "which is impossible - a straight line should not be trusted far from its data.\n";
    return 0;
}
