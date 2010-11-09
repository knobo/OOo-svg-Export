/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
 /*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * Contributors: Knut Olav Bøhmer <bohmer@gmail.com>
 *               aw@openoffice.org
 *               thb@openoffice.org
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
#ifndef INCLUDED_SVGEXPORT_HXX
#define INCLUDED_SVGEXPORT_HXX

#include <com/sun/star/document/XFilter.hpp>
#include <com/sun/star/document/XExporter.hpp>
#include <cppuhelper/implbase2.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>

class SVGFilter : public cppu::WeakImplHelper2< ::com::sun::star::document::XFilter,
                                                ::com::sun::star::document::XExporter >
{
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > mxMSF;
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >			 mxSrcDoc;
    
    // XFilter
    virtual ::sal_Bool SAL_CALL filter( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aDescriptor ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL cancel(  ) throw (::com::sun::star::uno::RuntimeException);

    // XExporter  
    virtual void SAL_CALL setSourceDocument( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >& Document ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
  
public:
    SVGFilter( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > &rxMSF );
};

#endif /*INCLUDED_SVGEXPORT_HXX*/

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
