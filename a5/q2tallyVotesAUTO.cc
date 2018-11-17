#include "q2tallyVotes.h"
#include "q2printer.h"
#include "q2voter.h"

using namespace std;

TallyVotes::TallyVotes( unsigned int voters, unsigned int group, Printer & printer )
    : full(false), voters(voters), group(group), numAvailVoters(voters), numWaitVoters(0), printer(printer) {}

TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ) {
    if ( numAvailVoters < group ) {
        _Throw Failed();
    } // if

    countVotes( ballot );
    printer.print( id, Voter::Vote, ballot );

    numWaitVoters++;
    if ( numWaitVoters < group ) {                                      // Check if this voter is the last one to form a group
        printer.print( id, Voter::Block, numWaitVoters );               // If NOT, wait for more voters coming
        WAITUNTIL( full, , );
        printer.print( id, Voter::Unblock, numWaitVoters - 1 );
    } else {
        printer.print( id, Voter::Complete );                           // If yes, print the complete message
        tour = bestVote();
        resetVotes();                                                   // Reset voting result for the next group, if this voter is the last one in a group
        full = true;
    } // if

    if ( numAvailVoters < group ) {
        _Throw Failed();
    } // if

    numWaitVoters--;
    if ( numWaitVoters == 0 ) {
        full = false;
    } // if
    
    RETURN( tour );
} // TallyVotes::vote

void TallyVotes::done() {
    numAvailVoters--;
    if ( numAvailVoters < group ) {
        full = true;
        RETURN();                                                       // unblock all voters
    }
} // TallyVotes::done