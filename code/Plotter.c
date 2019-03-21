

void Plotter()
{
	TCanvas* canvas = new TCanvas("Canvas","Canvas",1600,1800);
	canvas->Divide(1,3);
	
	// Make graph of scalers (if their maximum value is greater than 0 in any scaler interval)
	
	TGraph* scaler_graph[17]; // 16 channels from level translator, 1 for the ROI
	
	TMultiGraph* scaler_multigraph = new TMultiGraph();
	auto scaler_legend = new TLegend(0.9,0.6,1,0.9);
	
	for(int i=0;i<16;i++)
	{
		scaler_graph[i] = new TGraph(num_scaler_int,&time[0],&scaler[i][0]);
		defGraph(scaler_graph[i],"l",(i+1)*2,2,"Scalers","Time (sec)","Counts");
		if(*max_element(scaler[i].begin(),scaler[i].end())!=0)
		{
			scaler_multigraph->Add(scaler_graph[i]);
			scaler_legend->AddEntry(scaler_graph[i],scaler_name[i],"l");
		}
	}
	
	scaler_graph[16] = new TGraph(num_scaler_int,&time[0],&roi[0]);
	defGraph(scaler_graph[16],"l",(17)*2,2,"Scalers","Time (sec)","Counts");
	scaler_multigraph->Add(scaler_graph[16],"l");
	scaler_legend->AddEntry(scaler_graph[16],"ROI","l");
	
	// Make graph of currents (MFC1,MFC2,MFC1+MFC2, and MFC1/MFC2)
	
	TGraph* current_graph[4];
	
	TMultiGraph* current_multigraph = new TMultiGraph();
	auto current_legend = new TLegend(0.9,0.6,1,0.9);
	
	for(int i=0;i<3;i++)
	{
		current_graph[i] = new TGraph(num_scaler_int,&time[0],&current[i][0]);
		defGraph(current_graph[i],"l",(i+1)*2,2,"Current","Time (sec)","Current (uA)");
		current_multigraph->Add(current_graph[i],"l");
		current_legend->AddEntry(current_graph[i],current_name[i],"l");
	}
	
	current_graph[3] = new TGraph(num_scaler_int,&time[0],&current_ratio[0]);
	defGraph(current_graph[3],"l",(4)*2,2,"Current","Time (sec)","Current (uA)");
	current_multigraph->Add(current_graph[3],"l");
	current_legend->AddEntry(current_graph[3],current_name[3],"l");
	
	// Make graph of concentrations ( point-by-point, running average of individual concentration values, sum counts / sum currents, and the long interval for smoothing)
	
	TMultiGraph* concentration_multigraph = new TMultiGraph();
	auto concentration_legend = new TLegend(0.7,0.9,0.9,1);
	
	// Point-by-point
	
	TGraph* concentration_graph = new TGraph(num_scaler_int,&time[0],&concentration[0]);
	defGraph(concentration_graph,"l",1,2,"Concentration","Time (sec)","Concentration");
	concentration_multigraph->Add(concentration_graph,"l");
	
	concentration_legend->AddEntry(concentration_graph,"Point-by-Point","l");
	
	// Running Average
	
	TGraph* concentration_ave_graph = new TGraph(num_scaler_int,&time[0],&concentration_ave[0]);
	defGraph(concentration_ave_graph,"l",2,2,"Concentration","Time (sec)","Concentration");
	concentration_multigraph->Add(concentration_ave_graph,"l");
	
	TF1* concentration_ave_line = new TF1("concentration_ave",ave_line,0,run_time,1);
	TGraph* concentration_ave_line_graph = new TGraph(concentration_ave_line);
	defGraph(concentration_ave_line_graph,"l",2,2,"Concentration","Time (sec)","Concentration");
	concentration_multigraph->Add(concentration_ave_line_graph,"l");
	
	stringstream concentration_ave_graph_stream;
	concentration_ave_graph_stream << "Running Average: " << concentration_ave[num_scaler_int-1];
	TString concentration_ave_graph_string = concentration_ave_graph_stream.str();
	
	concentration_legend->AddEntry(concentration_ave_graph,concentration_ave_graph_string,"l");
	
	// Sum counts / sum currents
	
	TGraph* concentration_sum_graph = new TGraph(num_scaler_int,&time[0],&concentration_sum[0]);
	defGraph(concentration_sum_graph,"l",3,2,"Concentration","Time (sec)","Concentration");
	concentration_multigraph->Add(concentration_sum_graph,"l");
	
	TF1* concentration_sum_line = new TF1("concentration_sum",sum_line,0,run_time,1);
	TGraph* concentration_sum_line_graph = new TGraph(concentration_sum_line);
	defGraph(concentration_sum_line_graph,"l",3,2,"Concentration","Time (sec)","Concentration");
	concentration_multigraph->Add(concentration_sum_line_graph,"l");
	
	stringstream concentration_sum_graph_stream;
	concentration_sum_graph_stream << "Running Sum: " << concentration_sum[num_scaler_int-1];
	TString concentration_sum_graph_string = concentration_sum_graph_stream.str();
	
	concentration_legend->AddEntry(concentration_sum_graph,concentration_sum_graph_string,"l");
	
	// Long interval
	
	TGraph* concentration_graph_long_int = new TGraph(num_scaler_int*scaler_int/long_int,&time_long_int[0],&concentration_long_int[0]);
	defGraph(concentration_graph_long_int,"l",4,2,"Concentration","Time (sec)","Concentration");
	concentration_multigraph->Add(concentration_graph_long_int,"l");
	
	TBox* concentration_std_dev_range = new TBox(time[0],current_ave[num_scaler_int-1]-concentration_std_dev,time[num_scaler_int-1],current_ave[num_scaler_int-1]+concentration_std_dev);
	concentration_std_dev_range->SetFillColorAlpha(19,0.1);
	
	canvas->cd(1);
	scaler_multigraph->Draw("al");
	defMGraph(scaler_multigraph,"Scalers","Time (sec)","Counts");
	scaler_legend->Draw("same");
	
	canvas->cd(2);
	current_multigraph->Draw("al");
	defMGraph(current_multigraph,"Current","Time (sec)","Current (uA)");
	current_legend->Draw("same");
	
	TF2* scale_graph = new TF2("scale_graph",scale_graph_func,0,run_time,0,1e6,1);
	scale_graph->SetParameter(0,1e-6);
	
	for(int i=0;i<3;i++)
	{
		current_graph[i]->Apply(scale_graph);
	}
	
	Double_t max_current_display = 1.1*(*max_element(current[2].begin(),current[2].end()))/1e-6;
	current_multigraph->GetYaxis()->SetRangeUser(0,max_current_display);
	
	canvas->cd(3);
	concentration_multigraph->Draw("al");
	defMGraph(concentration_multigraph,"Concentration","Time (sec)","Concentration (10^(-15))");
	concentration_legend->Draw("same");
	concentration_std_dev_range->Draw("same");
	
	stringstream pdf_stream;
	pdf_stream << path_to_write << "run" << run_num << ".pdf";
	TString pdf_file = pdf_stream.str();
	canvas->SaveAs(pdf_file);
}

Double_t scale_graph_func(Double_t *val, Double_t *par)
{
   return val[1]/par[0];
}

Double_t ave_line(Double_t *val, Double_t *par)
{
	return concentration_ave[num_scaler_int-1];
}

Double_t sum_line(Double_t *val, Double_t *par)
{
	return concentration_sum[num_scaler_int-1];
}

void defGraph(TGraph* dGraph,TString markerType, Int_t markerColor, Int_t markerSize, TString Title, TString xTitle, TString yTitle)
{
	if(markerType == "l" || markerType == "L")
	{
		dGraph->SetLineColor(markerColor);
		dGraph->SetLineWidth(markerSize);
	}
	else if(markerType == "p" || markerType == "P")
	{
		dGraph->SetMarkerColor(markerColor);
		dGraph->SetMarkerSize(markerSize);
	}
	
	dGraph->SetTitle(Title);
	dGraph->GetXaxis()->SetTitle(xTitle);
	dGraph->GetYaxis()->SetTitle(yTitle);
	dGraph->GetXaxis()->CenterTitle();
	dGraph->GetXaxis()->SetTitleOffset(1.4);
	dGraph->GetXaxis()->SetLabelSize(.025);
	dGraph->GetYaxis()->CenterTitle();
	dGraph->GetYaxis()->SetTitleOffset(1.4);
	dGraph->GetYaxis()->SetLabelSize(.025);
}

void defMGraph(TMultiGraph* dGraph,TString Title, TString xTitle, TString yTitle)
{
	dGraph->SetTitle(Title);
	dGraph->GetXaxis()->SetTitle(xTitle);
	dGraph->GetYaxis()->SetTitle(yTitle);
	dGraph->GetXaxis()->CenterTitle();
	dGraph->GetXaxis()->SetTitleOffset(1.4);
	dGraph->GetXaxis()->SetLabelSize(.025);
	dGraph->GetYaxis()->CenterTitle();
	dGraph->GetYaxis()->SetTitleOffset(1.2);
	dGraph->GetYaxis()->SetLabelSize(.025);
}
