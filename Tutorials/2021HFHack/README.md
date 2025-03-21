# Practice folder

Recap HFHackTut exampels and practices

## Example1.cxx
	- Printing out particle id and momentum
	- Know how to subscribe to table
	- Know how to make a configurable

## Example2.cxx
	- Know how to make and fill histogram
	- Use HistogramRegistry in a straight way

	[[ Histogram definition ]]

	AxisSpec{nBins, rangeMin, rangeMax, "HistName(Option)"};	// -> Make axis( x-axis, y-axis...)
	HistogramConfigSpec{HistType, vector<AxisSpec>{}}; 			// -> Make histogram spec, hist type and axis definition
																//    This Histogram spec can be used in several individual histogram definition
	HistogramSpec{"Name", "Title", HistogramConfigSpec};		// -> Define individual histogram spec
																//    Histogram spec represents individual hist

	HistogramRegistry{"registry name", vector<HistogramSpec>{}};

## Example2-1.cxx
	- Use grouping -> 'Subscription to table iterator'
	- Know how to make and fill histogram
	- Use HistogramRegistry with certain procedures
		-- Define AxisSpec
		-- Use defined AxisSpec to define histogram at init() functin
	- Use get<T> method to get the histogram pointer

## Example3.cxx
	- Use filtering
	- Fill pT histogram of particles within certain eta range
		-- Use of alias for filtered table
		-- Group filtered tracks with same collision index, and calculated average pT

## Example4.cxx
	- Use table joining
	- Draw pT vs event multiplicity
	- To use event multiplicity table, helper task is needed

## o2-tutorial-handson2.cxx
	- Handson of O2Tutorial4.0
	- Apply quality cuts on Tracks table.
	  To do this, joining some tables to Tracks table is needed

	- Do the same task with Filter method

## Example5.cxx
	- Use Filter to Joined table
	- To apply quality cut to tracks, Join 'aod::Tracks' with 'aod::TracksExtra', 'aod::TracksDCA'
	- Apply filter to tracks: tpcNClsCrossedRows() 
	- Apply filter to tracks: dcaXY()
	
	- Do this with multiplicity at Example4.cxx

## Example6.cxx
	- Use Partition table
		-- Partitonined tables are not grouped by iterator consumed at process input
		-- For Partitioned tables, grouping must be done manually
			a) Using SliceCache
			b) Declaring Partition inside process function, and add matching of collision index as partitioning condition -> (Will be)Tested at Example6-0.cxx

	- Make Joined table for the events
	- Group particles with the collision iterator
		-- At this stage, partition particles with two sup-group

			a) Particles at mid-rapidity( -1.0 < eta < 1.0 )
			original plan:
				b) Particles at forward-rapidity - V0C
				c) Particles at forward-rapidity - V0A
				=> I think particles within this area are defined at another table aod::FwdTracks
			Revised:
			b)Particles outsie mid-rapidity
			c)MB( without adding any eta cut )

		-- Also, apply quality cuts on tracks( DCA cuts, and tpcNClsCrossedRows cut)

	- Fill histogram of pT and eta corresponds to each table
		
