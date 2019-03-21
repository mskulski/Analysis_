#include "TCanvas.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <vector>
#include <math.h>
#include <TMath.h>
#include <TFile.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TGraphErrors.h>
#include "TCut.h"
#include "TStyle.h"
#include "TColor.h"
#include <sstream> 
#include <fstream>
#include <stdlib.h>
#include "TString.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TGraphErrors.h"
using namespace std;

// call the system to load the .c files with the functions used in the analysis
	
gROOT->ProcessLine(".L ./code/Header.c");
gROOT->ProcessLine(".L ./code/CurrentAnalysis.c");	// reads the current files and fills current[i][j], current_ave[i][j], current_sum[i][j], current_long_int[i][j], and current_ratio[i]
gROOT->ProcessLine(".L ./code/ScalerAnalysis.c");	// creates the cuts from the cut files and fills scaler[i][j], scaler_sum[i][j], and scaler_long_int[i][j]
gROOT->ProcessLine(".L ./code/Plotter.c"); 			// creates plots for counts, currents, and concentrations
gROOT->ProcessLine(".L ./code/Writer.c");			// writes run data to file
gROOT->ProcessLine(".L ./code/CleanUp.c");			// clears variables, vectors (in case of wanting to re-run code back to back)

// declare variables

TString path_to_run_data = ""; 		// relative path to run .root files
TString path_to_current_data = ""; 	// relative path to current data files
TString path_to_cut_data = ""; 		// relative path to cut creation files
TString path_to_write = "";
TString cut_name = ""; 				// name of all cuts joined with &'s, used for output .txt file only

const int num_scalers = 16; // max number of scalers to consider, 16 is one level translator and all that is currently used
Int_t MFC_ref = 3; 			// MFC to reference for the concentration.  1=MFC1,2=MFC2,3=MFC1+MFC2 for two stable isotopes.  assigned in the header file
Int_t scaler_int = 1; 		// scaler interval for the run, integer number of seconds set in the readout program.  assigned in the header file
Int_t run_num; 				// run number being analyzed
Int_t num_scaler_int = 0; 	// number of scaler intervals in the run = run time / scaler interval
Int_t run_time = 0; 		// total run time
Int_t long_int; 			// time used to smooth the data for visual use only on the output plot.  assigned in the header file
Int_t num_cuts = 0; 		// number of cuts used in the analysis.  assigned in the header file
Int_t plot = 0;				// whether to plot the data (1) or not. assigned in the header file
Int_t write = 0;            // whether to write the data to file (1) or not. assigned in the header file

Double_t concentration_std_dev = 0;		// standard deviation of the point-by-point calculated concentrations

TString scaler_name[num_scalers] = {"LL","LR","","","RL","RR","AL","AR","Trig","TrigLive","","","MCP1","MCP2","Si",""};
TString current_name[4] = {"MFC1","MFC2","MFCsum","MFCratio"};

vector<vector<Double_t>> scaler(num_scalers);
vector<vector<Double_t>> scaler_sum(num_scalers);
vector<vector<Double_t>> scaler_long_int(num_scalers);

vector<Double_t> dead_time;
vector<Double_t> dead_time_sum;
vector<Double_t> dead_time_long_int;

vector<Double_t> event_scaler_interval;	// scaler interval number of each event in the ROI

vector<Double_t> current_ratio;	// current_ratio[i] = MFC1/MFC2 during scaler interval i (useless if two stable masses are not measured concurrently)

vector<Double_t> roi;
vector<Double_t> roi_sum;
vector<Double_t> roi_long_int;

vector<vector<Double_t>> current(3);
vector<vector<Double_t>> current_sum(3);
vector<vector<Double_t>> current_long_int(3);

vector<Double_t> concentration;
vector<Double_t> concentration_ave;
vector<Double_t> concentration_sum;
vector<Double_t> concentration_long_int;

vector<Double_t> time;
vector<Double_t> time_long_int;

vector<TString> cut_names;	// name of each cut.  assigned from header file

void Analysis(Int_t run)
{	
	run_num = run;
	
	Header();
	CurrentAnalysis();
	ScalerAnalysis();
	
	for(int i=0;i<num_scaler_int;i++)
	{
		concentration.push_back(1.6e-4*(roi[i])/(current[MFC_ref-1][i]));
		
		if(i>0)
		{
			concentration_ave.push_back((concentration_ave[i-1]*i + concentration[i])/(i+1));
		}
		else if(i == 0)
		{
			concentration_ave.push_back(concentration[0]);
		}
		
		concentration_sum.push_back(1.6e-4*(roi_sum[i])/(current_sum[MFC_ref-1][i]));
	}
	
	for(int i=0;i<num_scaler_int*scaler_int/long_int;i++)
	{
		concentration_long_int.push_back(1.6e-4*(roi_long_int[i])/(current_long_int[MFC_ref-1][i]));
	}
	
	for(int i=0;i<num_scaler_int;i++)
	{
		concentration_std_dev += pow(concentration[i]-concentration_ave[num_scaler_int-1],2);
	}
	
	concentration_std_dev = sqrt(concentration_std_dev/num_scaler_int);
	
	if(plot == 1)
	{
		Plotter();
	}
	
	if(write == 1)
	{
		Writer();
	}
	
	CleanUp();
}