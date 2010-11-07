class SVGFilter : public cppu::WeakImplHelper4 < XFilter,
                                                 XExporter >
{
  
private:
  
  Reference< XMultiServiceFactory >	mxMSF;
  Reference< XComponent >				mxSrcDoc;
    
  // XFilter
  virtual sal_Bool SAL_CALL filter( const Sequence< PropertyValue >& rDescriptor ) throw(RuntimeException);
  virtual void SAL_CALL cancel( ) throw (RuntimeException);
  

  // XExporter
  virtual void SAL_CALL setSourceDocument( const Reference< XComponent >& xDoc ) throw(IllegalArgumentException, RuntimeException);  
  
public:
  
  SVGFilter( const Reference< XMultiServiceFactory > &rxMSF );
  virtual		~SVGFilter();
};
