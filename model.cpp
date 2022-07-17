/* Implementácia projektu z predmetu IMS
 * Autor: Andrej Hýroš (xhyros@stud.fit.vutbr.cz)
 * Dátum: 12.12.21
 * VUT FIT v Brne
 */


#include <iostream>
#include <iomanip>
#include <cmath>
#include "Eigen/Dense"
#include "data.h"

#define PRINT(x) std::cout << x << std::endl
#define PRINT_INLINE(x) std::cout  << x
#define TEN_MILLIONS 10000000

using namespace Eigen;


//c alculates logarithms of elements of passed array
void ln_darr(double * input, double * output, int length) {
    for (int i = 0; i < length; i++) {
        output[i] = log(input[i]);
    }
}

// calculates percentual differnce between two doubles
int percent(double a, double b)
{
    double result = 0;
    result = ((b - a) * 100) / a;

    return int(result);
}

// calculates demand from available data using model:
// ln(demand_t) = b3 + b0*ln(price_t) + b1*ln(gdp_t) + b2*ln(population_t)
void calculate_demand(const MatrixXd& coeff, double * output, int datasize, double * price, double * gdp, double * pop) {
    double tmp;

    for (int i = 0; i < datasize; i++) {
        tmp = coeff(3, 0) + coeff(0, 0)* log(price[i]) + coeff(1, 0)*log(gdp[i]) + coeff(2, 0)*log(pop[i]);
        output[i] = exp(tmp)*TEN_MILLIONS;
    }
}

// prints real and predicted demand, and their absolute percentual difference
void print_model_vs_reality_differences(const double * real_data, double * predicted_data) {
    double diff;
    PRINT("EXPERIMENT 1 - VALIDATING MODEL (1980 - 2004)");
    std::cout << "YEAR\tREAL CONSUMPTION\tPREDICTED CONSUMPTION\tDIFFERENCE" << std::endl;
    for (int i = 0; i < DATA_LEN; i++) {
        diff = abs(percent(real_data[i]*TEN_MILLIONS, predicted_data[i]));
        std::cout << std::fixed;
        std::cout << std::setprecision(2) << std::setfill('0');
        PRINT_INLINE(years1[i]);
        PRINT_INLINE("\t\t");
        PRINT_INLINE(real_data[i]*TEN_MILLIONS);
        PRINT_INLINE("\t\t");
        PRINT_INLINE(predicted_data[i]);
        PRINT_INLINE("\t\t~");
        PRINT_INLINE(diff);
        PRINT_INLINE("%\n");
        //std::cout <<
        //years1[i] << "         " << real_data[i]*TEN_MILLIONS << "         " << predicted_data[i] << "         ~" << diff << "%" << std::endl;
    }

    PRINT("\n\n\n");

}

// prints output of experiment 2
void print_expertiment2(const double * demand) {
    PRINT("EXPERIMENT 2 - PREDICTION OF GAS DEMAND FOR 2020 - 2025");
    for (int i = 0; i < 6; i++) {
        std::cout << years2[i] << "\t" << demand[i] << " MCF" << std::endl;
    }
}

// calculates point estimates of regression coefficients using linear regression model
MatrixXd get_coefficient_point_estimates() {

    int r_coef_cnt = 4;                             // number of columns in plan matrix

    MatrixXd plan(DATA_LEN, r_coef_cnt);            // plan matrix
    MatrixXd planT(r_coef_cnt, DATA_LEN);           // transposed plan matrix
    MatrixXd H(plan.cols(), planT.rows());          // H matrix
    MatrixXd Hinv(plan.cols(), planT.rows());       // inverse H matrix
    MatrixXd b(r_coef_cnt, 1);                      // vector of point estimation of regress coefficients
    MatrixXd y(DATA_LEN, 1);                        // vector of ln(y) values

    double price_lnd[DATA_LEN];                     // arrays for logarithms of data
    double gdp_lnd[DATA_LEN];
    double population_lnd[DATA_LEN];
    double y_lnd[DATA_LEN];

    // filling arrays with logarithms of array elements
    ln_darr(PRICE, price_lnd, DATA_LEN);
    ln_darr(GDP, gdp_lnd, DATA_LEN);
    ln_darr(POPULATION, population_lnd, DATA_LEN);
    ln_darr(CONSUMPTION, y_lnd, DATA_LEN);

    // filling plan matrix with data
    for (int i = 0; i < DATA_LEN; i++) {
        plan(i, 0) = price_lnd[i];
        plan(i, 1) = gdp_lnd[i];
        plan(i, 2) = population_lnd[i];
        plan(i, 3) = 1.0;
    }

    // filling y with values
    for (int i = 0; i < DATA_LEN; i++) {
        y(i, 0) = y_lnd[i];
    }

    // transposing plan matrix
    planT = plan.transpose();

    // calculating H matrix as product of matrix multiplication
    H = planT * plan;

    // calculating H inverse
    Hinv = H.inverse();

    // calculating point estimates using: b = H^-1 * planT * y
    b = Hinv * planT;
    b = b * y;

    return b;
}


int main()
{
    double predicted_demand[DATA_LEN];

    // calculates point estimates of coefficients
    MatrixXd coefficients = get_coefficient_point_estimates();

    // calculate demand prediction using regression - experiment 1
    calculate_demand(coefficients, predicted_demand, DATA_LEN, PRICE, GDP, POPULATION);

    // print real vs modeled data (experiment 1)
    print_model_vs_reality_differences(CONSUMPTION, predicted_demand);

    // experiment 2
    calculate_demand(coefficients, predicted_demand, 6, prices, gdp, population);

    // print experiment 2
    print_expertiment2(predicted_demand);
}