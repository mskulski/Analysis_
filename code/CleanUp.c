

void CleanUp()
{
	concentration_std_dev = 0;
	
	dead_time.clear();
	dead_time_sum.clear();
	dead_time_long_int.clear();
	
	event_scaler_interval.clear();
	
	current_ratio.clear();
	
	roi.clear();
	roi_sum.clear();
	roi_long_int.clear();
	
	concentration.clear();
	concentration_ave.clear();
	concentration_sum.clear();
	concentration_long_int.clear();
	
	time.clear();
	time_long_int.clear();
	
	cut_names.clear();

	for(int i=0;i<num_scalers;i++)
	{
		scaler[i].clear();
		scaler_sum[i].clear();
		scaler_long_int[i].clear();
	}
	
	for(int i=0;i<3;i++)
	{
		current[i].clear();
		current_sum[i].clear();
		current_long_int[i].clear();
	}
}