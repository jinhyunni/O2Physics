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

## Example5.cxx
	- Use Filter to Joined table
	- To apply quality cut to tracks, Join 'aod::Tracks' with 'aod::TracksExtra', 'aod::TracksDCA'
	- Apply filter to tracks: tpcNClsCrossedRows() 
	- Apply filter to tracks: dcaXY()
	
	- Do this with multiplicity at Example4.cxx

## Example6.cxx
	- Use Partition table
		-- Partitoned tables are not grouped by iterator consumed at process input
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

## Example6-1.cxx
	- Use Partition table
		-- Partioned tables are not grouped by iterator at process input
		-- To grput partitioned tables, manual grouping is needed
			a) Using SliceCache -> See Example6.cxx
			b) Using .bindTable() method -> This method fills partioned table!

## Example7.cxx
	- Using MC data
	- To use anchord MC information, join additional table : aod::McTrackLabel
	- By using McTrackLabel index, aod::McParticles can be used
	- Joining aod::Tracks and aod::McParitcles are not joiniable, because they share getter with same name. This causes compile error

## Example8.cxx
	- Making derived table and consuming them
	- Objective : Make a joinable table with aod::McCollisions which contains information about multiplicity
	- Declared table name will be aod::MultsGen -> Multiplicity measured with generated particles

	- How to make tables
		-- Make namspaces to declare table and column( it is conventional that columns are defined at designated namespaces, and tables should be declared at o2::aod )
		-- Declare column with `DECLARE_SOA_COLUMN(Name, getter, type)` method to declare static column
		-- Declare table with `DECLARE_SOA_TABLE(Name, Origin, Description, column1, column2,...)`
			--- Origin is typically written as "AOD"
			--- Description can be a short sentence
			--- For column, specify getters with namespaces

## Example9.cxx
	- Making derived table and consuming them
