/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cds/threading/details/_common.h>
#include <cds/gc/hp.h>
#include <cds/gc/dhp.h>

namespace cds { namespace threading {

   atomics::atomic<size_t> ThreadData::s_nLastUsedProcNo( 0 );
   size_t ThreadData::s_nProcCount = 1;

   void ThreadData::init()
    {
        if ( m_nAttachCount++ == 0 ) {
            if ( cds::gc::HP::isUsed() )
                cds::gc::hp::smr::attach_thread();
            if ( cds::gc::DHP::isUsed() )
                cds::gc::dhp::smr::attach_thread();

            if ( cds::urcu::details::singleton<cds::urcu::general_instant_tag>::isUsed() )
                m_pGPIRCU = cds::urcu::details::singleton<cds::urcu::general_instant_tag>::attach_thread();
            if ( cds::urcu::details::singleton<cds::urcu::general_buffered_tag>::isUsed() )
                m_pGPBRCU = cds::urcu::details::singleton<cds::urcu::general_buffered_tag>::attach_thread();
            if ( cds::urcu::details::singleton<cds::urcu::general_threaded_tag>::isUsed() )
                m_pGPTRCU = cds::urcu::details::singleton<cds::urcu::general_threaded_tag>::attach_thread();
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
            if ( cds::urcu::details::singleton<cds::urcu::signal_buffered_tag>::isUsed() )
                m_pSHBRCU = cds::urcu::details::singleton<cds::urcu::signal_buffered_tag>::attach_thread();
#endif
        }
    }

   bool ThreadData::fini()
    {
        if ( --m_nAttachCount == 0 ) {
            if ( cds::gc::DHP::isUsed() )
                cds::gc::dhp::smr::detach_thread();
            if ( cds::gc::HP::isUsed() )
                cds::gc::hp::smr::detach_thread();

            if ( cds::urcu::details::singleton<cds::urcu::general_instant_tag>::isUsed() ) {
                cds::urcu::details::singleton<cds::urcu::general_instant_tag>::detach_thread( m_pGPIRCU );
                m_pGPIRCU = nullptr;
            }
            if ( cds::urcu::details::singleton<cds::urcu::general_buffered_tag>::isUsed() ) {
                cds::urcu::details::singleton<cds::urcu::general_buffered_tag>::detach_thread( m_pGPBRCU );
                m_pGPBRCU = nullptr;
            }
            if ( cds::urcu::details::singleton<cds::urcu::general_threaded_tag>::isUsed() ) {
                cds::urcu::details::singleton<cds::urcu::general_threaded_tag>::detach_thread( m_pGPTRCU );
                m_pGPTRCU = nullptr;
            }
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
            if ( cds::urcu::details::singleton<cds::urcu::signal_buffered_tag>::isUsed() ) {
                cds::urcu::details::singleton<cds::urcu::signal_buffered_tag>::detach_thread( m_pSHBRCU );
                m_pSHBRCU = nullptr;
            }
#endif
            return true;
        }
        return false;
    }


}} // namespace cds::threading
