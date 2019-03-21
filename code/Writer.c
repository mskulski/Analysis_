

void Writer()
{
	stringstream write_file_stream;
	write_file_stream << path_to_write << "Raw_Concentrations.txt";
	TString write_file_name = write_file_stream.str();
	
	std::ofstream write_file;
	write_file.open(write_file_name,std::ofstream::out|std::ofstream::app);
		
	write_file << run_num << " " << concentration_ave[num_scaler_int - 1] << " " << concentration_sum[num_scaler_int - 1] << " " << cut_name << " " << roi_sum[num_scaler_int-1] << " " << concentration_std_dev << endl;
	write_file.close();
}