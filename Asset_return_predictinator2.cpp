#include <iostream>
#include <cmath>
using namespace std;

const int MAXN = 100;

void buildX(double X[][4], double y[], int &n);
void transpose(double X[][4], int n, double Xt[][MAXN]);
void multiplyXtX(double Xt[][MAXN], double X[][4], int n, double XtX[][4]);
void multiplyXty(double Xt[][MAXN], double y[], int n, double Xty[]);
void invert4x4(double XtX[][4], double inv[][4]);
void computeBeta(double inv[][4], double Xty[4], double beta[4]);

int main() {
    cout << "This program LEARNS the coefficients (c, m1, m2, m3) from historical data using Ordinary Least Squares regression: beta = (X^T X)^-1 X^T y, then uses the learned model to predict future a[...]

    char answer = 'y';
    while (answer == 'y' || answer == 'Y') {


       double Xt[4][MAXN], X[MAXN][4], XtX[4][4], inv[4][4], y[MAXN], Xty[4], beta[4];
       int n = 0;

        buildX( X,  y,  n);
        transpose( X,  n,  Xt);
        multiplyXtX( Xt,  X, n,  XtX );
        multiplyXty( Xt,  y,  n,  Xty);
        invert4x4( XtX,  inv);
        computeBeta( inv,  Xty,  beta);

        double c  = beta[0];
        double m1 = beta[1];
        double m2 = beta[2];
        double m3 = beta[3];

        cout << "Now enter new market data to predict asset return:" << endl;  // one last input with pushing weighs c, m1, m2, m3

        double mt1new, mt0new;
        cout << "Enter market index price today: " << endl;
        cin >> mt1new;
        cout << "Enter market index price yesterday: " << endl;
        cin >> mt0new;
        double Rm_new = (mt1new-mt0new)/mt0new;

        double ret1new, ret2new, ret3new;
        cout << "Enter past three returns: ";
        cin >> ret1new >> ret2new >> ret3new;
        double momentum_new = ret1new + ret2new + ret3new;

        double mean_new= (ret1new + ret2new + ret3new)/3.0;
        double variance=(pow((ret1new - mean_new), 2.0) + pow((ret2new - mean_new), 2.0) + pow((ret3new - mean_new), 2.0)) / 3.0;
        double volatility_new= pow(variance, 0.5);

        double Rasset = c + m1 * Rm_new + m2 * momentum_new + m3 * volatility_new;

        cout << "Predicted asset return is " << Rasset << endl;

        cout << "Market return is used to measure how overall market is moving, momentum to observe trend and volatility to see asset fluctuations. " << endl;

        cout << "Do you want to make another calculation? y/n:";
        cin >> answer;
    }

    return 0;
}

void buildX(double X[][4], double y[], int &n) {

    cout << "How many historical observations will be provided for training? "; cin >> n;
    for (int i = 0; i < n; i++) {
        cout << "Observation " << i + 1<< " of " << n << endl;
        double mt1, mt0;
        cout << "Enter market index price today: " << endl;     // works across almost all currencies
        cin >> mt1;
        cout << "Enter market index price yesterday: " << endl;
        cin >> mt0;
        double Rm = (mt1-mt0)/mt0;

        double ret1, ret2, ret3;
        cout << "Enter past three returns: ";
        cin >> ret1 >> ret2 >> ret3;
        double momentum = ret1 + ret2 + ret3;

        double mean= (ret1 + ret2 + ret3)/3.0;
        double variance=(pow((ret1 - mean), 2.0) + pow((ret2 - mean), 2.0) + pow((ret3 - mean), 2.0)) / 3.0;
        double volatility= pow(variance, 0.5);

        double actualReturn;
        cout << "Enter the ACTUAL asset return that occurred in this period: ";
        cin >> actualReturn;

        // construction of matrix X and vector y
        X[i][0]= 1.0;                               // formula has a baseline piece c, the intercept
        X[i][1]= Rm;
        X[i][2]= momentum;
        X[i][3]= volatility;
        y[i]= actualReturn;
    }
}

void transpose(double X[][4], int n, double Xt[][MAXN]) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 4; j++) {
            Xt[j][i]= X[i][j];
        }
    }
}

void multiplyXtX(double Xt[][MAXN], double X[][4], int n, double XtX[][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            XtX[i][j] = 0;
            for (int k = 0; k < n; k++) {
                XtX[i][j] += Xt[i][k] * X[k][j];
            }
        }
    }
}

void multiplyXty(double Xt[][MAXN], double y[], int n, double Xty[]) {
    for (int i = 0; i < 4; i++) {
        Xty[i] = 0;
        for (int k = 0; k < n; k++) {
            Xty[i] += Xt[i][k] * y[k];
        }
    }
}

void invert4x4(double XtX[][4], double inv[][4]) {
    double aug[4][8];

    // build augmented matrix [A | I] (finding inverse by change to identity)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++)
            aug[i][j] = XtX[i][j];
        for (int j = 4; j < 8; j++)
            aug[i][j] = (j - 4 == i) ? 1.0 : 0.0;
    }

    // Gauss-Jordan elimination
    for (int i = 0; i < 4; i++) {
        double pivot = aug[i][i];

        if (fabs(pivot) < 1e-12) {
            for (int r = i + 1; r < 4; r++) {
                if (fabs(aug[r][i]) > 1e-12) {
                    for (int c = 0; c < 8; c++)
                        swap(aug[i][c], aug[r][c]);
                    pivot = aug[i][i];
                    break;
                }
            }
        }

        for (int c = 0; c < 8; c++)
            aug[i][c] /= pivot;

        for (int r = 0; r < 4; r++) {
            if (r != i) {
                double factor = aug[r][i];
                for (int c = 0; c < 8; c++)
                    aug[r][c] -= factor * aug[i][c];
            }
        }
    }

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            inv[i][j] = aug[i][j + 4];
}

void computeBeta(double inv[][4], double Xty[4], double beta[4]){
    for (int i = 0; i < 4; i++) {
        beta[i] = 0;
        for (int j = 0; j < 4; j++) {
            beta[i] += inv[i][j] * Xty[j];
        }
    }
}
