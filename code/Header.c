

void Header()
{
	std::ifstream analysis_header;
	analysis_header.open("./header/Analysis_Header.txt");	// header file that tells the code how to analyze the data.  comments in the header file will describe the lines required
	
	int read_lines = 0;
	
	while(1)
	{
		string line;
		getline(analysis_header,line,'#');	// a "#" must be put after inputs in Analysis_Header.txt to tell getline() to stop reading 
		analysis_header.ignore(1000,'\n');	// ignores the comments/text after "#" until the newline '\n'
		
		read_lines++;
		
		if(read_lines < 10)
		{
			switch(read_lines)
			{
				case 1: path_to_run_data = line;
						break;
					
				case 2: path_to_current_data = line;
						break;
					
				case 3: path_to_cut_data = line;
						break;
						
				case 4: path_to_write = line;
						break;
						
				case 5: scaler_int = atoi(line.c_str());
						break;
					
				case 6: MFC_ref = atoi(line.c_str());
						break;
					
				case 7: long_int = atoi(line.c_str());
						break;
					
				case 8: plot = atoi(line.c_str());
						break;
					
				case 9: write = atoi(line.c_str());
						break;
					
				case 10: num_cuts = atoi(line.c_str());
						break;
			}
		}
		else if( read_lines >= 11 && read_lines < num_cuts + 11)
		{
			cut_names.push_back(line);	// fills the vector with cut names for use in ScalerAnalysis
		}
		else
		{
			break;
		}
	}
	analysis_header.close();
	
	if(long_int%scaler_int != 0)
	{
		cout << "Time division selected not integer multiple of scaler interval, defaulting to long_int = scaler_int" << endl;
		long_int = scaler_int;
	}
}
