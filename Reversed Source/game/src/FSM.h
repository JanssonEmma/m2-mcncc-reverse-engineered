#ifndef _fsm_fsm_h
#define _fsm_fsm_h

// Reference-derived declaration checked against target DWARF and vtable.
// Target size is 44 bytes under its 32-bit GCC ABI.

// Local Includes
#include "state.h"

// FSM Class
class CFSM
{
	protected:
		CState *		m_pCurrentState;	// Current State
		CState *		m_pNewState;		// New State
		CStateTemplate<CFSM>	m_stateInitial;		// Initial State

	public:
		// Constructor
		CFSM();

		// Destructor
		virtual ~CFSM() {}

		// Global Functions
		virtual void Update();

		// State Functions
		bool IsState(CState &State);
		bool GotoState(CState &NewState);

		virtual void BeginStateInitial() {}
		virtual void StateInitial() {}
		virtual void EndStateInitial() {}
};

#endif
