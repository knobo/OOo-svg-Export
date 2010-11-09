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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_filter.hxx"

#include "svgexport2.hxx"

#include <comphelper/servicedecl.hxx>
#include <unotools/ucbstreamhelper.hxx>
#include <unotools/streamwrap.hxx>
#include <drawinglayer/primitive2d/baseprocesser2d.hxx>
#include <com/sun/star/io/XOutputStream.hpp>

using namespace ::com::sun::star;

namespace
{

class SVGRenderer : public drawinglayer::primitive2d::BaseProcesser2D
{
    virtual void processBasePrimitive2D(const primitive2d::BasePrimitive2D& rCandidate) 
    {
        switch(rCandidate.getPrimitive2DID())
        {
            case PRIMITIVE2D_ID_TRANSFORMPRIMITIVE2D :
            {
                // remember current ViewInformation2D
                const primitive2d::TransformPrimitive2D& rTransformCandidate(
                    static_cast< const primitive2d::TransformPrimitive2D& >(rCandidate));
                const geometry::ViewInformation2D aLastViewInformation2D(getViewInformation2D());
		  
                // create new local ViewInformation2D containing transformation
                const geometry::ViewInformation2D aViewInformation2D(
                    getViewInformation2D().getObjectTransformation() * rTransformCandidate.getTransformation(), 
                    getViewInformation2D().getViewTransformation(), 
                    getViewInformation2D().getViewport(),
                    getViewInformation2D().getVisualizedPage(),
                    getViewInformation2D().getViewTime(),
                    getViewInformation2D().getExtendedInformationSequence());
                updateViewInformation(aViewInformation2D);

                // proccess child content recursively
                process(rTransformCandidate.getChildren());

                // restore transformations
                updateViewInformation(aLastViewInformation2D);

                break;
            }

            case PRIMITIVE2D_ID_POLYGONHAIRLINEPRIMITIVE2D :
            {
                // create hairline in discrete coordinates
                const primitive2d::PolygonHairlinePrimitive2D& rPolygonCandidate(
                    static_cast< const primitive2d::PolygonHairlinePrimitive2D& >(rCandidate));
                // write PolygonHairlinePrimitive2D as svg
                // 
                break;
            }
		
            case PRIMITIVE2D_ID_POLYGONHAIRLINEPRIMITIVE2D :
            {
                break;
            }

            // - BitmapPrimitive2D (bitmap data, evtl. with transparence)
            // - PointArrayPrimitive2D (single points)
            // - PolygonHairlinePrimitive2D (hairline curves/polygons)
            // - PolyPolygonColorPrimitive2D (colored polygons)
		
            //    - TransparencePrimitive2D (objects with freely defined transparence) // needed
            //    - InvertPrimitive2D (for XOR)                                        // nearly not used (ignore)
            //    - MaskPrimitive2D (for masking)                                      // needed
            //    - ModifiedColorPrimitive2D (for a stack of color modifications)      // when you have shadow
            //    - TransformPrimitive2D (for a transformation stack)                  // needed
            
            default :
            {
                // process recursively
                process(rCandidate.get2DDecomposition(getViewInformation2D()));
                break;
            }
        }
    }

public:
    SVGRenderer(const geometry::ViewInformation2D& rViewInformation) : 
        drawinglayer::primitive2d::BaseProcesser2D(rViewInformation)
    {}
};

void convert_to_svg( const uno::Reference< drawing::XDrawPage >&   xDrawPage, 
					 const uno::sequence< graphics::Primitive2D >& xSequence,
					 const uno::Reference< io::XOutputStream >&    xOutStream ) 
{  
    // Open The file
    // Write svg header
    // page properties from XDrawPage
  
    // Iterate through primitives sequence
  
    // drawinglayer::primitive2d::Primitive2DSequence* pMySequence = 
    //	dynamic_cast< drawinglayer::primitive2d::Primitive2DSequence > ( xSequence );
  
    geometry::ViewInformation2D aViewInformation;
    SVGRenderer aRederer(aViewInformation);

    aRender.process(xSequence);

	// Two pass
    // Collect hasmap of gradients, colors, etc.
    // or keep track of 
}

} // anon namespace

SVGFilter::SVGFilter( const uno::Reference< lang::XMultiServiceFactory > &rxMSF ) :
    mxMSF(rxMSF),
	mxSrcDoc()
{}  
  
sal_Bool SAL_CALL SVGFilter::filter( const uno::Sequence< beans::PropertyValue >& rDescriptor ) throw(uno::RuntimeException)
{
    if( !mxSrcDoc.is() )
        throw uno::RuntimeException();

    uno::Reference< frame::XDesktop > xDesktop( 
        mxMSF->createInstance( 
            rtl::OUString( 
                RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.Desktop" ))), 
        uno::UNO_QUERY_THROW);

    uno::Reference< frame::XFrame > xFrame( xDesktop->getCurrentFrame() );
    if( !xFrame.is() )
        return false;

    uno::Reference< frame::XController > xController( xFrame->getController() );			
    if( !xController.is() )
        return false;

    uno::Reference< drawing::XDrawView > xDrawView( xController, uno::UNO_QUERY );
    if( !xDrawView.is() )
        return false;

    uno::Reference< drawing::XDrawPage > xDrawPage( xDrawView->getCurrentPage() );
    if( !xDrawPage.is() )
        return false;

    boost::scoped_ptr<SvStream>         pOStm;
    uno::Reference< io::XOutputStream > xOStm;
    sal_Int32							nLength = rDescriptor.getLength();
    const beans::PropertyValue*			pValue = rDescriptor.getConstArray();
    for( sal_Int32 i=0; i<nLength; ++i)
    {
        if( pValue[ i ].Name.equalsAscii( "OutputStream" ) )
            pValue[ i ].Value >>= xOStm;
        else if( pValue[ i ].Name.equalsAscii( "FileName" ) )
        {
            rtl::OUString aFileName;
            pValue[ i ].Value >>= aFileName;
            pOStm.set( ::utl::UcbStreamHelper::CreateStream( aFileName, STREAM_WRITE|STREAM_TRUNC ) );
            if( pOStm )
                xOStm = uno::Reference< io::XOutputStream >( new ::utl::OOutputStreamWrapper( *pOStm ));
        }
    }

    uno::Reference< graphic::PrimitiveFactory2D > xPrimitiveFactory2D ( 
        mxMSF->createInstance(
            rtl::OUString( 
                RTL_CONSTASCII_USTRINGPARAM("com.sun.star.graphic.PrimitiveFactory2D"))),
        uno::UNO_QUERY_THROW);

    uno::Sequence< graphics::Primitive2D > xPagePrimitives = 
        xPrimitiveFactory2D->createPrimitivesFromXDrawPage( 
            xDrawPage, 
            uno::Sequence<beans::PropertyValue>() );
    convert_to_svg(xDrawPage, xPagePrimitives, xOStm);
}

void SAL_CALL SVGFilter::cancel( ) throw (RuntimeException)
{}
  
void SAL_CALL SVGFilter::setSourceDocument( const Reference< XComponent >& xDoc ) throw(IllegalArgumentException, RuntimeException)
{
    mxSrcDoc = xDoc;
}

namespace sdecl = comphelper::service_decl;
sdecl::class_<SVGFilter> serviceImpl;
const sdecl::ServiceDecl svgExportDecl(
    serviceImpl,
    "com.sun.star.comp.Draw.SVGExport",
    "com.sun.star.document.ExportFilter" );

// The C shared lib entry points
COMPHELPER_SERVICEDECL_EXPORTS(svgExportDecl)
