// Reconstructed from game SHA-256 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c.
// Source organization and names guided by the newer reference tree.
#include <cassert>
#include "FSM.h"

// 0x08050040 / 0x08050180: base and complete constructors compared.
CFSM::CFSM()
{
    m_stateInitial.Set(this, &CFSM::BeginStateInitial,
                      &CFSM::StateInitial, &CFSM::EndStateInitial);
    m_pCurrentState = &m_stateInitial;
    m_pNewState = 0;
}

// 0x080502c0
void CFSM::Update()
{
    if (m_pNewState)
    {
        m_pCurrentState->ExecuteEndState();
        m_pCurrentState = m_pNewState;
        m_pNewState = 0;
        m_pCurrentState->ExecuteBeginState();
    }
    m_pCurrentState->ExecuteState();
}

// 0x08050310
bool CFSM::IsState(CState& state)
{
    return m_pCurrentState == &state;
}

// 0x08050330
bool CFSM::GotoState(CState& newState)
{
    if (!IsState(newState) || m_pNewState != &newState)
        m_pNewState = &newState;
    return true;
}
