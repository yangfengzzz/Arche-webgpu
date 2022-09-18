#ifdef HAS_BLENDSHAPE
	#ifdef HAS_BLENDSHAPE_TEXTURE	
		int vertexOffset = gl_VertexID * u_blendShapeTextureInfo.x;
		for(int i = 0; i < HAS_BLENDSHAPE_COUNT; i++){
			int vertexElementOffset = vertexOffset;
			float weight = blendShapeWeights[i];
			position.xyz += getBlendShapeVertexElement(i, vertexElementOffset) * weight;
			
			#ifndef OMIT_NORMAL
				#if defined( HAS_NORMAL ) && defined( HAS_BLENDSHAPE_NORMAL )
					vertexElementOffset += 1;
					normal += getBlendShapeVertexElement(i, vertexElementOffset) * weight;
				#endif

				#if defined( HAS_TANGENT ) && defined(HAS_BLENDSHAPE_TANGENT) && ( defined(HAS_NORMAL_TEXTURE) || defined(HAS_CLEARCOATNORMAL_TEXTURE) )
					vertexElementOffset += 1;
					tangent.xyz += getBlendShapeVertexElement(i, vertexElementOffset) * weight;
				#endif
			#endif
		}
	#else
		position.xyz += POSITION_BS0 * blendShapeWeights[0];
		position.xyz += POSITION_BS1 * blendShapeWeights[1];
		position.xyz += POSITION_BS2 * blendShapeWeights[2];
		position.xyz += POSITION_BS3 * blendShapeWeights[3];

		#if defined( HAS_BLENDSHAPE_NORMAL ) || defined( HAS_BLENDSHAPE_TANGENT )
			#ifndef OMIT_NORMAL
				#if defined( HAS_NORMAL ) && defined( HAS_BLENDSHAPE_NORMAL )
					normal += NORMAL_BS0 * blendShapeWeights[0];
					normal += NORMAL_BS1 * blendShapeWeights[1];
					normal += NORMAL_BS2 * blendShapeWeights[2];
					normal += NORMAL_BS3 * blendShapeWeights[3];
				#endif

				#if defined( HAS_TANGENT ) && defined(HAS_BLENDSHAPE_TANGENT) && ( defined(HAS_NORMAL_TEXTURE) || defined(HAS_CLEARCOATNORMAL_TEXTURE) )
					tangent.xyz += TANGENT_BS0 * blendShapeWeights[0];
					tangent.xyz += TANGENT_BS1 * blendShapeWeights[1];
					tangent.xyz += TANGENT_BS2 * blendShapeWeights[2];
					tangent.xyz += TANGENT_BS3 * blendShapeWeights[3];
				#endif
			#endif
		#else
			position.xyz += POSITION_BS4 * blendShapeWeights[4];
			position.xyz += POSITION_BS5 * blendShapeWeights[5];
			position.xyz += POSITION_BS6 * blendShapeWeights[6];
			position.xyz += POSITION_BS7 * blendShapeWeights[7];
		#endif
	#endif
#endif


