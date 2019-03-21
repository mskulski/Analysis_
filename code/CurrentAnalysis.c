

void CurrentAnalysis()
{
	stringstream current_stream;
	current_stream << path_to_current_data << "offset_current_run" << run_num << ".txt"; //name format used on the AccelNET computer to generate current files
	TString current_file = current_stream.str();
	std::ifstream current_i;
	
	current_i.open(current_file);
	
	Double_t dt = 0;
	Double_t nt = 0;
	Double_t dT = scaler_int;
	Double_t nT = 0;
	
	Double_t MFC[3];
	Double_t MFC_sum[3];
	
	while(1) //reads the offset current file (~0.1 s/measurement) and turns the current into an average per scaler interval
	{
		current_i >> dt >> MFC[0] >> MFC[1];
		
		MFC[0] = sqrt(MFC[0]*MFC[0]); //ensures the current is read as a positive value
		MFC[1] = sqrt(MFC[1]*MFC[1]);
		MFC[2] = MFC[0]+MFC[1];
		
		if(dt <= dT)
		{
			for(int i=0;i<3;i++)
			{
				MFC_sum[i] += MFC[i];
			}
			nt++;
		}
		else if(dt > dT)
		{
			for(int i=0;i<3;i++)
			{
				current[i].push_back(MFC_sum[i]/nt);
			}
			
			if(MFC_sum[1]/nt != 0)
			{
				current_ratio.push_back(MFC_sum[0]/MFC_sum[1]);
			}
			else
			{
				current_ratio.push_back(0);
			}
			
			if(nT > 0)
			{
				for(int i=0;i<3;i++)
				{
					current_sum[i].push_back(current_sum[i][nT-1] + current[i][nT]);
				}
			}
			else if(nT == 0)
			{
				for(int i=0;i<3;i++)
				{
					current_sum[i].push_back(MFC_sum[i]/nt);
				}
			}
			
			nT++;
			dT += scaler_int;
			
			nt = 1;
			
			for(int i=0;i<3;i++)
			{
				MFC_sum[i] = MFC[i];
			}
		}
		
		if(!current_i.good())
		{
			for(int i=0;i<3;i++)
			{
				current[i].push_back(MFC_sum[i]/nt);
			}
			
			if(MFC_sum[1]/nt != 0)
			{
				current_ratio.push_back(MFC_sum[0]/MFC_sum[1]);
			}
			else
			{
				current_ratio.push_back(0);
			}
			
			if(nT > 0)
			{
				for(int i=0;i<3;i++)
				{
					current_sum[i].push_back(current_sum[i][nT-1] + current[i][nT]);
				}
			}
			else if(nT == 0)
			{
				for(int i=0;i<3;i++)
				{
					current_sum[i].push_back(MFC_sum[i]/nt);
				}
			}
			
			nT++;
			dT += scaler_int;
			break;
		}
	}
	
	for(int i=0;i<nT*scaler_int/long_int;i++)
	{
		for(int j=0;j<3;j++)
		{
			if(i>0)
			{
				current_long_int[j].push_back(current_sum[j][(((i+1)*(long_int/scaler_int))-1)]-current_sum[j][(((i)*(long_int/scaler_int))-1)]);
			}
			else if(i==0)
			{
				current_long_int[j].push_back(current_sum[j][((i+1)*(long_int/scaler_int))-1]);
			}
		}
	}
}
