#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <queue>

using namespace std ;

#define MAX_LINE 1024
#define MAX_LEN 10

struct CPU {
  int id ;
  int burst ;
  int arrival ;
  int priority ;
  int done ;
};

struct Time {
  int id ;
  int waiting ;
  int turnaround ;
};

struct Mode {
	string modeName ;
	vector <Time> result ;
};

struct Prior {
  int prior ;
  vector <CPU> cpu ;
};

struct Ratio {
  float ratio ;
  CPU cpu ;
};

class Schedule {
  int method ;
  int slice ;
  vector <CPU> cpu ;
  vector <string> Gantt ;
  vector <Mode> mode ;
  
  int GetSlice( string record ) {
    int i = 0 ;
    int num = 0, temp = 0 ;
    for ( ; i < record.size() ; i++ ) {
      if ( record[i] == ' ' )
        break ;
    }
    
    for ( ; i < record.size() ; i++ ) {
      if ( record[i] >'0' && record[i] < '9' )
        break ;
    }
    
    while ( record[i] != '\t' && record[i] != '\n' && record[i] != ' ' ) { 
			temp = record[i] - '0' ;
			num = num * 10 + temp ;
			i++ ;
		}
		
		return num ;
  } // GetSlice()
  
  template <class T>
  void Swap( T & a, T & b ) {
		T temp = b ;
		b = a ;
		a = temp ;
	}
	
	void sortId( vector <Time> & set ) {		
		for ( int last = set.size()-1 ; last > 0 ; last = last - 1 ) {
			int largest = 0 ;
			for ( int index = 1 ; index < last + 1 ; index = index + 1 ) {
				if ( set.at( index ).id > set.at( largest ).id )
					largest = index ;
			}
			if ( largest != last )
				Swap( set.at( largest ), set.at( last ) ) ;
		}
	}
	
	void sort_burst( vector <CPU> & set ) {
		for ( int last = set.size()-1 ; last > 0 ; last = last - 1 ) {
			int largest = 0 ;
			for ( int index = 1 ; index < last + 1 ; index = index + 1 ) {
				if ( ( set.at( index ).burst - set.at( index ).done ) > ( set.at( largest ).burst - set.at( largest ).done ) )
					largest = index ;
				else if ( ( set.at( index ).burst - set.at( index ).done ) == ( set.at( largest ).burst - set.at( largest ).done ) )
					if ( set.at( index ).arrival > set.at( largest ).arrival )
						largest = index ;
					else if ( set.at( index ).arrival == set.at( largest ).arrival )
					 	if ( set.at( index ).id > set.at( largest ).id )
						  largest = index ;
			}
			if ( largest != last )
				Swap( set.at( largest ), set.at( last ) ) ;
		}
		
		/*
    cout << method << " " << slice << endl ;
    for ( int i = 0 ; i < set.size() ; i++ ) {
      cout << set.at( i ).id << "	" << set.at( i ).burst << "	" ;
      cout << set.at( i ).arrival << "	" << set.at( i ).priority << endl ;
    }
    */
	} // Sort()
	
	void sort_prior( vector <Prior> & set ) {
	  for ( int last = set.size()-1 ; last > 0 ; last = last - 1 ) {
			int largest = 0 ;
			for ( int index = 1 ; index < last + 1 ; index = index + 1 ) {
				if ( set.at( index ).prior > set.at( largest ).prior )
					largest = index ;
			}
			if ( largest != last )
				Swap( set.at( largest ), set.at( last ) ) ;
		}
  }
  
  void sort_ratio( int time, vector <Ratio> & set ) {
	  for ( int i = 0 ; i < set.size() ; i++ ) {
	    set.at( i ).ratio = GetRatio( time, set.at( i ).cpu ) ;
    } // for
	  
		for ( int last = set.size()-1 ; last > 0 ; last = last - 1 ) {
			int largest = 0 ;
			for ( int index = 1 ; index < last + 1 ; index = index + 1 ) {
				if ( set.at( index ).ratio < set.at( largest ).ratio )
					largest = index ;
				else if ( set.at( index ).ratio == set.at( largest ).ratio )
					if ( set.at( index ).cpu.arrival > set.at( largest ).cpu.arrival )
						largest = index ;
					else if ( set.at( index ).cpu.arrival == set.at( largest ).cpu.arrival )
					 	if ( set.at( index ).cpu.id > set.at( largest ).cpu.id )
						  largest = index ;
			}
			if ( largest != last )
				Swap( set.at( largest ), set.at( last ) ) ;
		}
		
		/*
    for ( int i = 0 ; i < set.size() ; i++ ) {
      cout << set.at( i ).cpu.id << "	" << set.at( i ).cpu.burst << "	" ;
      cout << set.at( i ).cpu.arrival << "	" << set.at( i ).ratio << endl ;
    }
    cout << "-----------------\n" ;
    */
    
	} // Sort()
	
	void Sort( vector <CPU> & set ) {
		for ( int last = set.size()-1 ; last > 0 ; last = last - 1 ) {
			int largest = 0 ;
			for ( int index = 1 ; index < last + 1 ; index = index + 1 ) {
				if ( set.at( index ).arrival > set.at( largest ).arrival )
					largest = index ;
				else if ( set.at( index ).arrival == set.at( largest ).arrival )
					if ( set.at( index ).id > set.at( largest ).id )
						largest = index ;
			}
			if ( largest != last )
				Swap( set.at( largest ), set.at( last ) ) ;
		}
		
		/*
    cout << method << " " << slice << endl ;
    for ( int i = 0 ; i < set.size() ; i++ ) {
      cout << set.at( i ).id << "	" << set.at( i ).burst << "	" ;
      cout << set.at( i ).arrival << "	" << set.at( i ).priority << endl ;
    }
    */
	} // Sort()
  
  char GetCodename( int id ) {
		if ( id >= 10 )
			return (char)('A' + ( id-10 ) ) ;
		else
			return (char)( id + '0' ) ;
	}
	
	void FCFS( vector <Time> & result ) {
		int time = 0, i = 0 ;
		string process = "" ;
		vector <CPU> sort ;
		queue <CPU> que ;
		sort.assign( cpu.begin(), cpu.end() ) ;
		Sort( sort ) ;
		while ( i < sort.size() ) {
			while ( !que.empty() &&  i < sort.size() ) {				// make queue process to be scheduling
				CPU cur = que.front() ;
				que.pop() ;
        char code = GetCodename( cur.id ) ;
				for ( int j = cur.burst ; j > 0 ; j-- )
					process = process + code ;
						
				time = time + cur.burst ;	
				Time temp ;
				temp.id = cur.id ;
				temp.turnaround = time - cur.arrival ;
				temp.waiting = temp.turnaround - cur.burst ;
				result.push_back( temp ) ;      
        if ( time >= sort.at( i ).arrival )
          break ;				
			} // while
							
			if ( sort.at( i ).arrival - time > 0 ) {
			  for ( int j = sort.at( i ).arrival - time ; j > 0 ; j-- )
					process = process + "-" ;
					
				time = sort.at( i ).arrival ;	
      } // if			
			
			while ( i < sort.size() && sort.at( i ).arrival <= time ) {
				que.push( sort.at( i ) ) ;
				i++ ; 
			} // while
		} // while
		
		while ( !que.empty() ) {				// make queue process to be scheduling
			CPU cur = que.front() ;
			que.pop() ;
      char code = GetCodename( cur.id ) ;
			for ( int j = cur.burst ; j > 0 ; j-- )
				process = process + code ;
						
			time = time + cur.burst ;	
			Time temp ;
			temp.id = cur.id ;
			temp.turnaround = time - cur.arrival ;
			temp.waiting = temp.turnaround - cur.burst ;
			result.push_back( temp ) ;			
		} // while
		
		Gantt.push_back( process ) ;
		sortId( result ) ;
	}
	
	void RR( vector <Time> & result ) {
		int time = 0, i = 0 ;
		int timeSlice = 0 ;
		string process = "" ;
		vector <CPU> sort ;
		queue <CPU> que ;
		sort.assign( cpu.begin(), cpu.end() ) ;
		Sort( sort ) ;
		while ( i < sort.size() ) {	
			while ( !que.empty() &&  i < sort.size() ) {				// make queue process to be scheduling
				CPU cur = que.front() ;
				que.pop() ;	
				if ( cur.burst - cur.done > slice ) {
					time = time + slice ;
					char code = GetCodename( cur.id ) ;
					for ( int j = slice ; j > 0 ; j-- )
						process = process + code ;
				
					cur.done = cur.done + slice ;
				} // if
				else {
					time = time + ( cur.burst - cur.done ) ;
					char code = GetCodename( cur.id ) ;
					for ( int j = cur.burst - cur.done ; j > 0 ; j-- )
						process = process + code ;
				
					cur.done = cur.burst ;
				
					Time temp ;
					temp.id = cur.id ;
					temp.turnaround = time - cur.arrival ;
					temp.waiting = temp.turnaround - cur.burst ;
					result.push_back( temp ) ;
				} // else
				
				while ( i < sort.size() && sort.at( i ).arrival <= time ) {		
					que.push( sort.at( i ) ) ;
					i++ ; 
				} // while
				
				if ( cur.done < cur.burst )
					que.push( cur ) ;
					
			} // while
							
			if ( i < sort.size() ) {
				if ( sort.at( i ).arrival - time > 0 ) 
					for ( int j = sort.at( i ).arrival - time ; j > 0 ; j-- )
						process = process + "-" ;
				
				if ( sort.at( i ).burst - sort.at( i ).done > slice ) {
					time = sort.at( i ).arrival + slice ;
					char code = GetCodename( sort.at( i ).id ) ;
					for ( int j = slice ; j > 0 ; j-- )
						process = process + code ;
					
					sort.at( i ).done = sort.at( i ).done + slice ;
				} // if
			  else {
					time = sort.at( i ).arrival + ( sort.at( i ).burst - sort.at( i ).done ) ;
					char code = GetCodename( sort.at( i ).id ) ;
					for ( int j = sort.at( i ).burst - sort.at( i ).done ; j > 0 ; j-- )
						process = process + code ;
					
					sort.at( i ).done = sort.at( i ).burst ;
					
					Time temp ;
					temp.id = sort.at( i ).id ;
					temp.turnaround = time - sort.at( i ).arrival ;
					temp.waiting = temp.turnaround - sort.at( i ).burst ;
					result.push_back( temp ) ;
				} // else
										
				int j = i ;
				i++ ;
				while ( i < sort.size() && sort.at( i ).arrival <= time ) {
					que.push( sort.at( i ) ) ;
					i++ ; 
				} // while
					
				if ( sort.at( j ).done < sort.at( j ).burst )
					que.push( sort.at( j ) ) ;

			} // if		
		} // while
		
		while ( !que.empty() ) {				// make queue process to be scheduling
			CPU cur = que.front() ;
			que.pop() ;	
			if ( cur.burst - cur.done > slice ) {
				time = time + slice ;
				char code = GetCodename( cur.id ) ;
				for ( int j = slice ; j > 0 ; j-- )
					process = process + code ;
					
				cur.done = cur.done + slice ;
			} // if
			else {
				time = time + ( cur.burst - cur.done ) ;
				char code = GetCodename( cur.id ) ;
				for ( int j = cur.burst - cur.done ; j > 0 ; j-- )
					process = process + code ;
			
				cur.done = cur.burst ;
			
				Time temp ;
				temp.id = cur.id ;
				temp.turnaround = time - cur.arrival ;
				temp.waiting = temp.turnaround - cur.burst ;
				result.push_back( temp ) ;
			} // else
			
			if ( cur.done < cur.burst )
				que.push( cur ) ;
		} // while
		Gantt.push_back( process ) ;
		sortId( result ) ;
	} // RR()
	
	void SRTF( vector <Time> & result ) {
		int time = 0, i = 0 ;
		string process = "" ;
		vector <CPU> sort ;
		vector <CPU> que ;
		sort.assign( cpu.begin(), cpu.end() ) ;
		Sort( sort ) ;
		while ( i < sort.size() ) {
			while ( !que.empty() &&  i < sort.size() ) {				// make queue process to be scheduling
				CPU cur = que.at( 0 ) ;
				que.erase( que.begin() ) ;
        if ( time + ( cur.burst - cur.done ) > sort.at( i ).arrival ) {    
          char code = GetCodename( cur.id ) ;
					for ( int j = sort.at( i ).arrival - time ; j > 0 ; j-- )
						process = process + code ;
          
          cur.done = cur.done + sort.at( i ).arrival - time ;      
          que.push_back( cur ) ;
          time = sort.at( i ).arrival ;         
          break ;
        } // if
        else {
          char code = GetCodename( cur.id ) ;
					for ( int j = cur.burst - cur.done ; j > 0 ; j-- )
						process = process + code ;
						
					time = time + ( cur.burst - cur.done ) ;	
					Time temp ;
					temp.id = cur.id ;
					temp.turnaround = time - cur.arrival ;
					temp.waiting = temp.turnaround - cur.burst ;
					result.push_back( temp ) ;
        } // else				
			} // while
							
			if ( sort.at( i ).arrival - time > 0 ) 
				for ( int j = sort.at( i ).arrival - time ; j > 0 ; j-- )
					process = process + "-" ;
					
			time = sort.at( i ).arrival ;	
			while ( i < sort.size() && sort.at( i ).arrival == time ) {
				que.push_back( sort.at( i ) ) ;
				i++ ; 
			} // while
				
			sort_burst( que ) ;
		} // while
		
		while ( !que.empty() ) {				// make queue process to be scheduling
			CPU cur = que.at( 0 ) ;
			que.erase( que.begin() ) ;
      char code = GetCodename( cur.id ) ;
			for ( int j = cur.burst - cur.done ; j > 0 ; j-- )
				process = process + code ;
						
			time = time + ( cur.burst - cur.done ) ;	
			Time temp ;
			temp.id = cur.id ;
			temp.turnaround = time - cur.arrival ;
			temp.waiting = temp.turnaround - cur.burst ;
			result.push_back( temp ) ;			
		} // while
		
		Gantt.push_back( process ) ;
		sortId( result ) ;
	}
	
	void PPRR( vector <Time> & result ) {
		int time = 0, i = 0, curSlice = 0 ;
		string process = "" ;
		vector <CPU> sort ;
		vector <Prior> que ;
		sort.assign( cpu.begin(), cpu.end() ) ;
		Sort( sort ) ;
		while ( i < sort.size() ) {
		  while ( !que.empty() && i < sort.size() ) {
        CPU cur = que.at( 0 ).cpu.at( 0 ) ;
        que.at( 0 ).cpu.erase( que.at( 0 ).cpu.begin() ) ;
        if ( que.at( 0 ).cpu.size() == 0 )
          que.erase( que.begin() ) ;
          
        if ( time + ( slice - curSlice ) > sort.at( i ).arrival ) {     // before timeout, new process is coming
          cur.done = cur.done + sort.at( i ).arrival - time ;
		      curSlice = sort.at( i ).arrival - time ;
		      char code = GetCodename( cur.id ) ;
				  for ( int j = sort.at( i ).arrival - time ; j > 0 ; j-- )
						process = process + code ;
						  
		      time = sort.at( i ).arrival ; 
		      int biggest = 999 ;
		      for ( int j = i ; j < sort.size() ; j++ ) {
		        if ( time == sort.at( j ).arrival ) {
		          if ( sort.at( j ).priority < biggest )
		            biggest = sort.at( j ).priority ;
            } // if
            else
              break ;
          } // for
		        
		      if ( biggest < cur.priority ) {       // be preemptive
		        for (; i < sort.size() ; i++ ) {
		          if ( time == sort.at( i ).arrival ) {
		            Insert( que, sort.at( i ), false ) ;
              } // if
              else
                break ;
            } // for
            Insert( que, cur, false ) ;
            curSlice = 0 ; 
              sort_prior( que ) ;
          } // if
          else {
            Insert( que, cur, true ) ;     
            for (; i < sort.size() ; i++ ) {
		          if ( time == sort.at( i ).arrival ) {
		            Insert( que, sort.at( i ), false ) ;
              } // if
              else
                break ;
            } // for
              
            sort_prior( que ) ;
          } // else
        }
        else if ( cur.burst - cur.done > ( slice - curSlice ) ) {
          char code = GetCodename( cur.id ) ;
					for ( int j = slice - curSlice ; j > 0 ; j-- )
						process = process + code ; 
                
          time = time + ( slice - curSlice ) ;
          cur.done = cur.done + ( slice - curSlice ) ;
          curSlice = 0 ;
          for (; i < sort.size() ; i++ ) {
		        if ( time == sort.at( i ).arrival ) {
		          Insert( que, sort.at( i ), false ) ;
            } // if
            else
              break ;
          } // for
          
          Insert( que, cur, false ) ;
          sort_prior( que ) ;
        } // else if
        else {
          curSlice = 0 ;
          char code = GetCodename( cur.id ) ;
					for ( int j = cur.burst - cur.done ; j > 0 ; j-- )
						process = process + code ; 
            
          time = time + ( cur.burst - cur.done ) ;
          Time temp ;
					temp.id = cur.id ;
					temp.turnaround = time - cur.arrival ;
					temp.waiting = temp.turnaround - cur.burst ;
					result.push_back( temp ) ;
        } // else
      } // while
   
      if ( i < sort.size() ) {
        if ( sort.at( i ).arrival - time > 0 ) {
			    for ( int j = sort.at( i ).arrival - time ; j > 0 ; j-- )
					  process = process + "-" ;
			    time = sort.at( i ).arrival ;		
        }
	
			  while ( i < sort.size() && sort.at( i ).arrival == time ) {
			    Prior temp ;
			    Insert( que, sort.at( i ), false ) ;
				  i++ ; 
			  } // while
			
			  sort_prior( que ) ;
      } // if
    }
    
    while ( !que.empty() ) {       
      CPU cur = que.at( 0 ).cpu.at( 0 ) ;   
      que.at( 0 ).cpu.erase( que.at( 0 ).cpu.begin() ) ;
      if ( que.at( 0 ).cpu.size() == 0 )
        que.erase( que.begin() ) ;
		  if ( cur.burst - cur.done > ( slice - curSlice ) ) {        // timeout        
				char code = GetCodename( cur.id ) ;
				for ( int j = slice - curSlice ; j > 0 ; j-- )
					process = process + code ; 
                
        time = time + ( slice - curSlice ) ;
        cur.done = cur.done + ( slice - curSlice ) ;
        curSlice = 0 ;
        Insert( que, cur, false ) ;
			} // if
			else {        // no timeout	      
        char code = GetCodename( cur.id ) ;
				for ( int j = cur.burst - cur.done ; j > 0 ; j-- )
					process = process + code ; 
            
        time = time + ( cur.burst - cur.done ) ;
        curSlice = 0 ;
        Time temp ;
				temp.id = cur.id ;
				temp.turnaround = time - cur.arrival ;
				temp.waiting = temp.turnaround - cur.burst ;
				result.push_back( temp ) ;
			} // else
    } // while
    
    Gantt.push_back( process ) ;
		sortId( result ) ;
	}
	
	void HRRN( vector <Time> & result ) {
		int time = 0, i = 0 ;
		string process = "" ;
		vector <CPU> sort ;
		vector <Ratio> que ;
		sort.assign( cpu.begin(), cpu.end() ) ;
		Sort( sort ) ;
		while ( i < sort.size() ) {
			while ( !que.empty() &&  i < sort.size() ) {				// make queue process to be scheduling
        Ratio cur = que.at( 0 ) ;
				que.erase( que.begin() ) ;
        char code = GetCodename( cur.cpu.id ) ;
				for ( int j = cur.cpu.burst ; j > 0 ; j-- )
					process = process + code ;
						
				time = time + cur.cpu.burst ;	
				Time temp ;
				temp.id = cur.cpu.id ;
				temp.turnaround = time - cur.cpu.arrival ;
				temp.waiting = temp.turnaround - cur.cpu.burst ;
				result.push_back( temp ) ;
        if ( time >= sort.at( i ).arrival )
          break ;
        else
          sort_ratio( time, que ) ;			
			} // while
							
			if ( sort.at( i ).arrival - time > 0 ) {
			  for ( int j = sort.at( i ).arrival - time ; j > 0 ; j-- )
					process = process + "-" ;
			  time = sort.at( i ).arrival ;		
      }
	
			while ( i < sort.size() && sort.at( i ).arrival <= time ) {
			  Ratio temp ;
			  temp.cpu = sort.at( i ) ;
			  temp.ratio = GetRatio( time, temp.cpu ) ;
				que.push_back( temp ) ;
				i++ ; 
			} // while
				
			sort_ratio( time, que ) ;
		} // while
		
		while ( !que.empty() ) {				// make queue process to be scheduling
			Ratio cur = que.at( 0 ) ;
			que.erase( que.begin() ) ;
      char code = GetCodename( cur.cpu.id ) ;
			for ( int j = cur.cpu.burst ; j > 0 ; j-- )
				process = process + code ;
						
			time = time + cur.cpu.burst ;	
			Time temp ;
			temp.id = cur.cpu.id ;
			temp.turnaround = time - cur.cpu.arrival ;
			temp.waiting = temp.turnaround - cur.cpu.burst ;
			result.push_back( temp ) ;
      
      sort_ratio( time, que ) ;		
		} // while
		
		Gantt.push_back( process ) ;
		sortId( result ) ;
	}
	
	void Insert( vector <Prior> & set, CPU cpu, bool isFront ) {
	  bool insert = false ;
	  for ( int i = 0 ; i < set.size() ; i++ ) {
	    if ( cpu.priority == set.at( i ).prior ) {
        if ( isFront ) {
          set.at( i ).cpu.insert( set.at( i ).cpu.begin(), cpu ) ;
        } // if
        else {
          set.at( i ).cpu.push_back( cpu ) ;
        } // else
        
        insert = true ;
      } // if   
    } // for
    
    if ( !insert ) {
      Prior temp ;
	    temp.cpu.push_back( cpu ) ;
	    temp.prior = cpu.priority ;
	    set.push_back( temp ) ;
    } // if
    
    sort_prior( set ) ;
  } // Insert()
	
	float GetRatio( int time, CPU cpu ) {
	  return (float)( ( time - cpu.arrival ) + cpu.burst ) / cpu.burst ;
  }
  
  string GetMethod( int type ) {
  	if ( type == 1 )
  		return "FCFS" ;
  	else if ( type == 2 )
  		return "RR" ;
  	else if ( type == 3 )
  		return "SRTF" ;
  	else if ( type == 4 )
  		return "PPRR" ;
  	else if ( type == 5 )
  		return "HRRN" ;
  	else if ( type == 6 )
  		return "All" ;
  	else
  		return "" ;
	} // GetMethod()
public:
  bool readF( string fileName ) {
    FILE *infile = NULL ;							// a file handle	
		char * name = new char[MAX_LINE+1] ;
		fileName = fileName + ".txt" ;
		infile = fopen( fileName.c_str(), "r" ) ;		// open the file
		if ( infile == NULL ) {
			cout << endl << "### " << fileName << " does not exist! ###" << endl ;
			return false ;
		}
		else {
			CPU temp ;
      fscanf( infile, "%d", &method ) ;
      fscanf( infile, "%d", &slice ) ;
      fgets( name,MAX_LINE,infile ) ;
      fgets( name,MAX_LINE,infile ) ;
			while( fscanf( infile, "%d", &temp.id ) != EOF ) {		
        fscanf( infile, "%d", &temp.burst ) ;
        fscanf( infile, "%d", &temp.arrival ) ;
        fscanf( infile, "%d", &temp.priority ) ;
        temp.done = 0 ;
				cpu.push_back( temp ) ;
		  }
    }
    
    fclose( infile ) ;
    /*
    cout << method << " " << slice << endl ;
    for ( int i = 0 ; i < cpu.size() ; i++ ) {
      cout << cpu.at( i ).id << "	" << cpu.at( i ).burst << "	" ;
      cout << cpu.at( i ).arrival << "	" << cpu.at( i ).priority << endl ;
    }
    */
    return true ;
  } // readF()
  
  void writeF( string fileName) {
  	FILE * outfile ;
		Time temp ;				
		outfile = fopen( fileName.c_str(), "w" ) ;
		if ( method <= 5 ) {
			fprintf( outfile, "%s\n%s\n", mode.at( 0 ).modeName.c_str(), Gantt.at( 0 ).c_str() ) ;
			fprintf( outfile, "===========================================================\n\n" ) ;
			fprintf( outfile, "waiting\nID	" ) ;
			fprintf( outfile, "%s\n", mode.at( 0 ).modeName.c_str() ) ;
			fprintf( outfile, "===========================================================\n" ) ;
			for ( int i = 0 ; i < mode.at( 0 ).result.size() ; i++ ) {
				temp = mode.at( 0 ).result.at( i ) ;
				fprintf( outfile, "%d	%d\n", temp.id, temp.waiting ) ;
			} // for
			
			fprintf( outfile, "===========================================================\n" ) ;
			fprintf( outfile, "\nTurnaround Time\nID	" ) ;
			fprintf( outfile, "%s\n", mode.at( 0 ).modeName.c_str() ) ;
			fprintf( outfile, "===========================================================\n" ) ;
			for ( int i = 0 ; i < mode.at( 0 ).result.size() ; i++ ) {
				temp = mode.at( 0 ).result.at( i ) ;
				fprintf( outfile, "%d	%d\n", temp.id, temp.turnaround ) ;
			} // for
			fprintf( outfile, "===========================================================\n\n" ) ;
		}
		else {
		  fprintf( outfile, "All\n" ) ;
		  fprintf( outfile, "==        FCFS==\n" ) ;
      fprintf( outfile, "%s\n%", Gantt.at( 0 ).c_str() ) ;
      fprintf( outfile, "==          RR==\n" ) ;
      fprintf( outfile, "%s\n%", Gantt.at( 1 ).c_str() ) ;
      fprintf( outfile, "==        SRTF==\n" ) ;
      fprintf( outfile, "%s\n%", Gantt.at( 2 ).c_str() ) ;
      fprintf( outfile, "==        PPRR==\n" ) ;
      fprintf( outfile, "%s\n%", Gantt.at( 3 ).c_str() ) ;
      fprintf( outfile, "==        HRRN==\n" ) ;
      fprintf( outfile, "%s\n%", Gantt.at( 4 ).c_str() ) ;
			fprintf( outfile, "===========================================================\n\n" ) ;
			fprintf( outfile, "waiting\n" ) ;
			fprintf( outfile, "ID	FCFS	RR	SRTF	PPRR	HRRN\n" ) ;
			fprintf( outfile, "===========================================================\n" ) ;
			for ( int i = 0 ; i < mode.at( 0 ).result.size() ; i++ ) {
				fprintf( outfile, "%d	%d", mode.at( 0 ).result.at( i ).id, mode.at( 0 ).result.at( i ).waiting ) ;
				fprintf( outfile, "	%d	%d", mode.at( 1 ).result.at( i ).waiting, mode.at( 2 ).result.at( i ).waiting ) ;
				fprintf( outfile, "	%d	%d\n", mode.at( 3 ).result.at( i ).waiting, mode.at( 4 ).result.at( i ).waiting ) ;
			} // for
			fprintf( outfile, "===========================================================\n" ) ;
			fprintf( outfile, "\nTurnaround Time\n" ) ;
			fprintf( outfile, "ID	FCFS	RR	SRTF	PPRR	HRRN\n" ) ;
			fprintf( outfile, "===========================================================\n" ) ;
			for ( int i = 0 ; i < mode.at( 0 ).result.size() ; i++ ) {
				fprintf( outfile, "%d	%d", mode.at( 0 ).result.at( i ).id, mode.at( 0 ).result.at( i ).turnaround ) ;
				fprintf( outfile, "	%d	%d", mode.at( 1 ).result.at( i ).turnaround, mode.at( 2 ).result.at( i ).turnaround ) ;
				fprintf( outfile, "	%d	%d\n", mode.at( 3 ).result.at( i ).turnaround, mode.at( 4 ).result.at( i ).turnaround ) ;
			} // for
			fprintf( outfile, "===========================================================\n\n" ) ;
		}
		
		fclose( outfile ) ;
	} 
	
	bool make() {
		Mode temp ;
		if ( method >= 1 && method <= 5 ) {
			temp.modeName = GetMethod( method ) ;
			if ( method == 1 )
				FCFS( temp.result ) ;
			else if ( method == 2 )
				RR( temp.result ) ;
			else if ( method == 3 )
				SRTF( temp.result ) ;
			else if ( method == 4 )
				PPRR( temp.result ) ;
			else if ( method == 5 )
				HRRN( temp.result ) ;
			mode.push_back( temp ) ;
			return true ;
		} // if
		else if ( method == 6 ) {
			temp.modeName = GetMethod( 1 ) ;
			FCFS( temp.result ) ;
			mode.push_back( temp ) ;
			temp.result.clear() ;
			temp.modeName = GetMethod( 2 ) ;
			RR( temp.result ) ;
			mode.push_back( temp ) ;
			temp.result.clear() ;
			temp.modeName = GetMethod( 3 ) ;
			SRTF( temp.result ) ;
			mode.push_back( temp ) ;
			temp.result.clear() ;
			temp.modeName = GetMethod( 4 ) ;
			PPRR( temp.result ) ;
			mode.push_back( temp ) ;
			temp.result.clear() ;
			temp.modeName = GetMethod( 5 ) ;
			HRRN( temp.result ) ;
			mode.push_back( temp ) ;
			return true ;
		} // else if
		else
		  cout << endl << "### Error : the method " << method << " is not correct(1-6) ###" << endl ;
		return false ;
	}
  
  bool isEmpty() {
    if ( cpu.size() == 0 )
      return true ;
    else
      return false ;
  }
  
  void clearUp() {
    method = 0 ;
    slice = 0 ;
    cpu.clear() ;
    mode.clear() ;
    Gantt.clear() ;
  }
};



int main( void ) {
	string command;
	Schedule schedule ;
	do {
		string fileName ;
		cout << endl << "*** CPU Scheduling ***" ;
		cout << endl << "* 0. EXIT            *" ;
		cout << endl << "* 1. Read File       *" ;
		cout << endl << "**********************" ;
		cout << endl << "Input a choice(0, 1): " ;
		cin >> command ;
		cin.ignore( MAX_LEN, '\n' ) ;
		if ( command.compare( "1" ) == 0 ) {
			schedule.clearUp() ;
			do {
				cout << endl << "Input a file name ([0] Quit): " ;
				cin >> fileName ;
				if ( fileName.compare( "0" ) == 0 ) 
					break ;
				else if ( schedule.readF( fileName ) ) {
					if ( !schedule.isEmpty() ) {
						bool success = schedule.make() ; 
						if ( success ) {
						  fileName = "out_"+ fileName + ".txt" ;
              schedule.writeF( fileName ) ;
            }
                          
						break ;
					} // if
					else
						cout << endl << "### There is no data! ###" << endl ;							
				}
			} while ( 1 ) ;
		}
		else if ( command.compare( "0" ) == 0 )	
			break ;
		else
			cout << endl << "Command does not exist!" << endl ;	
	} while ( true );
	return 0 ;
}
