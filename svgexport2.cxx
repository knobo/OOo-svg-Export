SVGFilter::SVGFilter( const Reference< XMultiServiceFactory > &rxMSF ) :
    mxMSF(rxMSF),
	mxSrcDoc()
  {
  }

  
SVGFilter::~SVGFilter()
{
}
  
  
  // XFilter
sal_Bool SAL_CALL SVGFilter::filter( const Sequence< PropertyValue >& rDescriptor ) throw(RuntimeException){

  if( mxSrcDoc.is() )
	{
	  uno::Reference< frame::XDesktop > xDesktop( mxMSF->createInstance( ::rtl::OUString::createFromAscii( "com.sun.star.frame.Desktop" ) ), 
												  uno::UNO_QUERY);
		if( xDesktop.is() )
		{
			uno::Reference< frame::XFrame > xFrame( xDesktop->getCurrentFrame() );
			
			if( xFrame.is() )
			{
				uno::Reference< frame::XController > xController( xFrame->getController() );
				
				if( xController.is() )
				{
					uno::Reference< drawing::XDrawView > xDrawView( xController, uno::UNO_QUERY );
					
					if( xDrawView.is() )
					{
					  uno::Reference< drawing::XDrawPage > xDrawPage( xDrawView->getCurrentPage() );
						
						if( xDrawPage.is() )
						{
						  Reference< XOutputStream > xOStm;
						  SvStream*                  pOStm = NULL;

						  for ( sal_Int32 i = 0 ; i < nLength; ++i)
							{
							  if( pValue[ i ].Name.equalsAscii( "OutputStream" ) )
								pValue[ i ].Value >>= xOStm;
							  else if( pValue[ i ].Name.equalsAscii( "FileName" ) )
								{
								  ::rtl::OUString aFileName;
								  
								  pValue[ i ].Value >>= aFileName;
								  pOStm = ::utl::UcbStreamHelper::CreateStream( aFileName, STREAM_WRITE | STREAM_TRUNC );
								  
								  if( pOStm )
									xOStm = Reference< XOutputStream >( new ::utl::OOutputStreamWrapper ( *pOStm ) );
								}
							} // end for 
						  // com::sun::star::uno::sequence< com.sun.star.graphics.Primitive2D >
						  uno::Reference< graphic::PrimitiveFactory2D > xPrimitiveFactory2D ( mxMSF->createInstance(
																	::rtl::OUString::createFromAscii( "com.sun.star.graphic.PrimitiveFactory2D" )),
																							  uno::UNO_QUERY);
						  uno::Sequence < beans::PropertyValue > xEmpty;
						  if (xPrimitiveFactory2D.is()){
							com::sun::star::uno::sequence< com.sun.star.graphics.Primitive2D > xSequence = 
							  xPrimitiveFactory2D->createPrimitivesFromXDrawPage( xDrawPage, xEmpty );
						  }
						  
						  if (xSequence.is()){
							convert_to_svg(xDrawPage, xSequence, aFileName);
						  }
						}
					}
				}
			}
		}
	}
}


class myRenderer:public drawinglayer::primitive2d::BaseProcesser2D
{
protected:
  
  virtual void processBasePrimitive2D(const primitive2d::BasePrimitive2D& rCandidate) {

	switch(rCandidate.getPrimitive2DID())
	  {
	  case PRIMITIVE2D_ID_TRANSFORMPRIMITIVE2D :
		{
		  // remember current ViewInformation2D
		  const primitive2d::TransformPrimitive2D& rTransformCandidate(static_cast< const primitive2d::TransformPrimitive2D& >(rCandidate));
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
		  // http://docs.go-oo.org/drawinglayer/html/classdrawinglayer_1_1primitive2d_1_1PolygonHairlinePrimitive2D.html
		  // create hairline in discrete coordinates
		  const primitive2d::PolygonHairlinePrimitive2D& rPolygonCandidate(static_cast< const primitive2d::PolygonHairlinePrimitive2D& >(rCandidate));
		  // write PolygonHairlinePrimitive2D as svg
		  // 
		  break;
		}
		
	  case PRIMITIVE2D_ID_POLYGONHAIRLINEPRIMITIVE2D :
		{
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
  myRenderer(const geometry::ViewInformation2D& rViewInformation):drawinglayer::primitive2d::BaseProcesser2D(rViewInformation){
	
  }

  virtual ~myRenderer(){
	
  }

};


void convert_to_svg( const uno::Reference< drawing::XDrawPage >& xDrawPage, 
					 const uno::sequence< graphics::Primitive2D >& xSequence,
					 const ::rtl::OUString& aFileName ) {
  
  // Open The file
  // Write svg header
  // page properties from XDrawPage
  
  // Iterate through primitives sequence
  
  // drawinglayer::primitive2d::Primitive2DSequence* pMySequence = 
  //	dynamic_cast< drawinglayer::primitive2d::Primitive2DSequence > ( xSequence );
  
  geometry::ViewInformation2D aViewInformation;
  myRenderer aRederer(aViewInformation);

  aRender.process(xSequence);

	// Two pass
  // Collect hasmap of gradients, colors, etc.
  // or keep track of 
}

void SAL_CALL SVGFilter::cancel( ) throw (RuntimeException){
  
}
  

// XExporter
void SAL_CALL SVGFilter::setSourceDocument( const Reference< XComponent >& xDoc ) throw(IllegalArgumentException, RuntimeException){
  mxSrcDoc = xDoc;
}
  


// link against drawing layer
//
//
