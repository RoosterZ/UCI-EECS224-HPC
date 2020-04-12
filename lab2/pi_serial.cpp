#include<stdio.h>
#include<iostream>
#include<omp.h>
#include<bits/stdc++.h>

static long num_steps = 5e8;
double step;

using namespace std;

int main(){
    double pi = 0.0;
    double sum;
    double x;

    step = 1.0 / (double) num_steps;
    double initTime = opm_get_wtime();
    for (int i = 0; i < num_steps; i++){
        x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);

    }
    pi = step * sum;
    double endTime = omp_get_wtime();
    double time = endTime - initTime;

    cout<<"pi value is "<<pi<<endl;
    cout<<"Time serial is "<<time<<endl;

}


