

void ScalerAnalysis()
{
	stringstream runstream; //dynamically creates the root file name from run_n for opening the file
	runstream << path_to_run_data << "run" << run_num << ".root";
	TString file = runstream.str();
	
	cout << file << endl;
	
	TFile *tfile = TFile::Open(file);
	
	TCutG* cutg[20]; //cuts for spectra - 20 is chosen since the real number should be less (increase if needed)
	TCut* cut[20];
	
	stringstream cut_name_stream;
	
	Int_t n_tcut = 0;
	Int_t n_tcutg = 0;
	
	for(int i=0;i<num_cuts;i++)
	{
		stringstream cut_stream;
		cut_stream << "./cuts/" << cut_names[i] << ".txt";
		TString cut_file = cut_stream.str();
		std::ifstream cut_i;
		cut_i.open(cut_file);
	
		Int_t dimension;
		Int_t n_points;
		TString var_x;
		TString var_y;
		
		TString x_var;
		TString y_var;
		
		cut_i >> dimension >> n_points;
		
		if(i>0)
		{
			cut_name_stream << "&&";
		}
		
		cut_name_stream << cut_names[i];
		
		if(dimension == 1)
		{
			if(n_points < 2)
			{
				cout << "2 points required for a 1-D histogram!" << endl;
			}
			if(n_points > 2)
			{
				cout << "Only 2 points can be used for a 1-D histogram!  Proceeding with the first two points given." << endl;
			}
			
			cut_i >> var_x; 
			
			Double_t x_val_1;
			Double_t x_val_2;
			
			cut_i >> x_val_1;
			cut_i >> x_val_2;
			
			Double_t center = 0.5*(x_val_1 + x_val_2);
			Double_t range = sqrt((x_val_1 - center)*(x_val_1 - center));
			
			stringstream tcut_stream;
			
			tcut_stream << "sqrt((" << var_x << "-" << center << ")*(" << var_x << "-" << center << "))<" << range;
			cut[n_tcut] = tcut_stream.str();
			cut[n_tcut]->SetName(cut_names[i]);
			n_tcut++;
		}
		else if(dimension == 2)
		{
			cutg[n_tcutg] = new TCutG(cut_names[i],n_points);
			
			cut_i >> var_x >> var_y; 
			
			cutg[n_tcutg]->SetVarX(var_x);
			cutg[n_tcutg]->SetVarY(var_y);
			
			Double_t x_val;
			Double_t y_val;
			
			cut_i >> x_val >> y_val;
			
			for(int k=0;k<n_points;k++)
			{
				Double_t x_val;
				Double_t y_val;
		
				cut_i >> x_val >> y_val;
		
				cutg[n_tcutg]->SetPoint(k,x_val,y_val);
			}
			
			n_tcutg++;
		}
		
		if(i == num_cuts - 1)
		{
			cut_name = cut_name_stream.str();
		}
	}
	
	evtTree->Draw(">>evtlist",cut_name,"goff");
	
	Int_t num_events;
	
	if(evtlist)
	{
		num_events = evtlist->GetN();
	}
	
	vector<Int_t> unused_events;
	
	for(int i=0;i<num_events;i++)
	{
		unused_events.push_back(i);
	}
	
	num_scaler_int = scalerTree->GetEntries()-1; // the last scaler entry is the overflow from the DAQ shutting down, so is not within the window set in the run time
	run_time = num_scaler_int*scaler_int;
	const int scaler_ref = 10;
	
	for(int j=0;j<num_events;j++)
	{
		Int_t event_number = evtlist->GetEntry(j);
			
		evtTree->GetEvent(event_number);
			
		Double_t event_scaler_number = evtTree->GetLeaf("scaler_number")->GetValue();
		
		event_scaler_interval.push_back(event_scaler_number);	
	}
	
	for(int i=0;i<num_scaler_int;i++)
	{		
		Double_t event_counts = 0;
			
		for(int j=0;j<unused_events.size();j++)
		{
			Int_t event_number = evtlist->GetEntry(unused_events[j]);
			
			evtTree->GetEvent(event_number);
		
			Double_t event_scaler_number = evtTree->GetLeaf("scaler_number")->GetValue();
					
			if(event_scaler_number == i)
			{
				event_counts++;
				unused_events.erase(unused_events.begin()+j);
				j--;
			}
		}
		
		roi.push_back(event_counts);
		
		if(i>0)
		{
			roi_sum.push_back((roi_sum[i-1])+(roi[i]));
		}
		
		else if(i==0)
		{
			roi_sum.push_back(roi[i]);
		}
	}

	for(int i=0;i<num_scaler_int;i++)
	{
		time.push_back(i*scaler_int);
		
		scalerTree->GetEvent(i);
		
		for(int j=0;j<16;j++)
		{
			Double_t scaler_n = scalerTree->GetLeaf("scalers")->GetValue(j);
			scaler[j].push_back(scaler_n);
			
			if(i>0)
			{
				scaler_sum[j].push_back((scaler_sum[j][i-1])+(scaler_n));
			}
			else if(i==0)
			{
				scaler_sum[j].push_back(scaler_n);
			}
		}
		
		dead_time.push_back(scaler[8][i]/scaler[9][i]);
		dead_time_sum.push_back(scaler_sum[8][i]/scaler_sum[9][i]);
	}
	
	for(int i=0;i<num_scaler_int*scaler_int/long_int;i++)
	{
		time_long_int.push_back((i+0.5)*long_int);
		
		if(i>0)
		{
			roi_long_int.push_back((roi_sum[(((i+1)*(long_int/scaler_int))-1)])-(roi_sum[(((i)*(long_int/scaler_int))-1)]));
			
			Double_t trigger_total_difference = (scaler_sum[8][(((i+1)*(long_int/scaler_int))-1)]) - (scaler_sum[8][(((i)*(long_int/scaler_int))-1)]);
			Double_t trigger_live_difference = (scaler_sum[9][(((i+1)*(long_int/scaler_int))-1)]) - (scaler_sum[9][(((i)*(long_int/scaler_int))-1)]);
			
			dead_time_long_int.push_back(trigger_total_difference/trigger_live_difference);
		}
		else if(i==0)
		{
			roi_long_int.push_back(roi_sum[((i+1)*(long_int/scaler_int))-1]);
			dead_time_long_int.push_back((scaler_sum[8][((i+1)*(long_int/scaler_int))-1])/(scaler_sum[9][((i+1)*(long_int/scaler_int))-1]));
		}
	}
}