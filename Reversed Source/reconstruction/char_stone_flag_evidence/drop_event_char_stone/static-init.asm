
PROJECT_ROOT\Original Binaries\game:     file format elf32-i386


Disassembly of section .text:

08198cd0 <_Z41__static_initialization_and_destruction_0ii>:
 8198cd0:	55                   	push   ebp
 8198cd1:	89 e5                	mov    ebp,esp
 8198cd3:	83 ec 48             	sub    esp,0x48
 8198cd6:	89 7d fc             	mov    DWORD PTR [ebp-0x4],edi
 8198cd9:	8b 7d 0c             	mov    edi,DWORD PTR [ebp+0xc]
 8198cdc:	89 5d f4             	mov    DWORD PTR [ebp-0xc],ebx
 8198cdf:	8b 5d 08             	mov    ebx,DWORD PTR [ebp+0x8]
 8198ce2:	81 ff ff ff 00 00    	cmp    edi,0xffff
 8198ce8:	89 75 f8             	mov    DWORD PTR [ebp-0x8],esi
 8198ceb:	74 13                	je     8198d00 <_Z41__static_initialization_and_destruction_0ii+0x30>
 8198ced:	8b 5d f4             	mov    ebx,DWORD PTR [ebp-0xc]
 8198cf0:	8b 75 f8             	mov    esi,DWORD PTR [ebp-0x8]
 8198cf3:	8b 7d fc             	mov    edi,DWORD PTR [ebp-0x4]
 8198cf6:	89 ec                	mov    esp,ebp
 8198cf8:	5d                   	pop    ebp
 8198cf9:	c3                   	ret
 8198cfa:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
 8198d00:	83 fb 01             	cmp    ebx,0x1
 8198d03:	0f 84 31 01 00 00    	je     8198e3a <_Z41__static_initialization_and_destruction_0ii+0x16a>
 8198d09:	81 ff ff ff 00 00    	cmp    edi,0xffff
 8198d0f:	75 dc                	jne    8198ced <_Z41__static_initialization_and_destruction_0ii+0x1d>
 8198d11:	83 fb 01             	cmp    ebx,0x1
 8198d14:	0f 84 e9 00 00 00    	je     8198e03 <_Z41__static_initialization_and_destruction_0ii+0x133>
 8198d1a:	85 db                	test   ebx,ebx
 8198d1c:	75 54                	jne    8198d72 <_Z41__static_initialization_and_destruction_0ii+0xa2>
 8198d1e:	b8 80 d2 42 08       	mov    eax,0x842d280
 8198d23:	85 c0                	test   eax,eax
 8198d25:	74 4b                	je     8198d72 <_Z41__static_initialization_and_destruction_0ii+0xa2>
 8198d27:	be c8 d2 42 08       	mov    esi,0x842d2c8
 8198d2c:	81 fe 80 d2 42 08    	cmp    esi,0x842d280
 8198d32:	74 3e                	je     8198d72 <_Z41__static_initialization_and_destruction_0ii+0xa2>
 8198d34:	83 ee 0c             	sub    esi,0xc
 8198d37:	8b 0e                	mov    ecx,DWORD PTR [esi]
 8198d39:	8b 56 04             	mov    edx,DWORD PTR [esi+0x4]
 8198d3c:	89 c8                	mov    eax,ecx
 8198d3e:	39 d1                	cmp    ecx,edx
 8198d40:	74 15                	je     8198d57 <_Z41__static_initialization_and_destruction_0ii+0x87>
 8198d42:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
 8198d49:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]
 8198d50:	83 c0 10             	add    eax,0x10
 8198d53:	39 d0                	cmp    eax,edx
 8198d55:	75 f9                	jne    8198d50 <_Z41__static_initialization_and_destruction_0ii+0x80>
 8198d57:	8b 46 08             	mov    eax,DWORD PTR [esi+0x8]
 8198d5a:	29 c8                	sub    eax,ecx
 8198d5c:	c1 f8 04             	sar    eax,0x4
 8198d5f:	74 cb                	je     8198d2c <_Z41__static_initialization_and_destruction_0ii+0x5c>
 8198d61:	89 0c 24             	mov    DWORD PTR [esp],ecx
 8198d64:	c1 e0 04             	shl    eax,0x4
 8198d67:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
 8198d6b:	e8 3c 39 eb ff       	call   804c6ac <_ZNSt24__default_alloc_templateILb1ELi0EE10deallocateEPvj@plt>
 8198d70:	eb ba                	jmp    8198d2c <_Z41__static_initialization_and_destruction_0ii+0x5c>
 8198d72:	81 ff ff ff 00 00    	cmp    edi,0xffff
 8198d78:	0f 85 6f ff ff ff    	jne    8198ced <_Z41__static_initialization_and_destruction_0ii+0x1d>
 8198d7e:	85 db                	test   ebx,ebx
 8198d80:	75 1f                	jne    8198da1 <_Z41__static_initialization_and_destruction_0ii+0xd1>
 8198d82:	a1 68 d2 42 08       	mov    eax,ds:0x842d268
 8198d87:	85 c0                	test   eax,eax
 8198d89:	75 3b                	jne    8198dc6 <_Z41__static_initialization_and_destruction_0ii+0xf6>
 8198d8b:	b8 14 00 00 00       	mov    eax,0x14
 8198d90:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
 8198d94:	a1 64 d2 42 08       	mov    eax,ds:0x842d264
 8198d99:	89 04 24             	mov    DWORD PTR [esp],eax
 8198d9c:	e8 0b 39 eb ff       	call   804c6ac <_ZNSt24__default_alloc_templateILb1ELi0EE10deallocateEPvj@plt>
 8198da1:	81 ff ff ff 00 00    	cmp    edi,0xffff
 8198da7:	0f 85 40 ff ff ff    	jne    8198ced <_Z41__static_initialization_and_destruction_0ii+0x1d>
 8198dad:	85 db                	test   ebx,ebx
 8198daf:	0f 85 38 ff ff ff    	jne    8198ced <_Z41__static_initialization_and_destruction_0ii+0x1d>
 8198db5:	c7 04 24 60 d2 42 08 	mov    DWORD PTR [esp],0x842d260
 8198dbc:	e8 fb 39 eb ff       	call   804c7bc <_ZNSt8ios_base4InitD1Ev@plt>
 8198dc1:	e9 27 ff ff ff       	jmp    8198ced <_Z41__static_initialization_and_destruction_0ii+0x1d>
 8198dc6:	a1 64 d2 42 08       	mov    eax,ds:0x842d264
 8198dcb:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
 8198dce:	c7 04 24 64 d2 42 08 	mov    DWORD PTR [esp],0x842d264
 8198dd5:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
 8198dd9:	e8 22 7f ef ff       	call   8090d00 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE>
 8198dde:	a1 64 d2 42 08       	mov    eax,ds:0x842d264
 8198de3:	89 40 08             	mov    DWORD PTR [eax+0x8],eax
 8198de6:	a1 64 d2 42 08       	mov    eax,ds:0x842d264
 8198deb:	c7 40 04 00 00 00 00 	mov    DWORD PTR [eax+0x4],0x0
 8198df2:	a1 64 d2 42 08       	mov    eax,ds:0x842d264
 8198df7:	89 40 0c             	mov    DWORD PTR [eax+0xc],eax
 8198dfa:	31 c0                	xor    eax,eax
 8198dfc:	a3 68 d2 42 08       	mov    ds:0x842d268,eax
 8198e01:	eb 88                	jmp    8198d8b <_Z41__static_initialization_and_destruction_0ii+0xbb>
 8198e03:	c6 05 d8 d2 42 08 00 	mov    BYTE PTR ds:0x842d2d8,0x0
 8198e0a:	b8 64 00 00 00       	mov    eax,0x64
 8198e0f:	be c8 00 00 00       	mov    esi,0xc8
 8198e14:	a3 c8 d2 42 08       	mov    ds:0x842d2c8,eax
 8198e19:	b9 2c 01 00 00       	mov    ecx,0x12c
 8198e1e:	ba 0a 00 00 00       	mov    edx,0xa
 8198e23:	89 35 cc d2 42 08    	mov    DWORD PTR ds:0x842d2cc,esi
 8198e29:	89 0d d0 d2 42 08    	mov    DWORD PTR ds:0x842d2d0,ecx
 8198e2f:	89 15 d4 d2 42 08    	mov    DWORD PTR ds:0x842d2d4,edx
 8198e35:	e9 e0 fe ff ff       	jmp    8198d1a <_Z41__static_initialization_and_destruction_0ii+0x4a>
 8198e3a:	c7 04 24 60 d2 42 08 	mov    DWORD PTR [esp],0x842d260
 8198e41:	e8 76 36 eb ff       	call   804c4bc <_ZNSt8ios_base4InitC1Ev@plt>
 8198e46:	83 fb 01             	cmp    ebx,0x1
 8198e49:	0f 85 ba fe ff ff    	jne    8198d09 <_Z41__static_initialization_and_destruction_0ii+0x39>
 8198e4f:	c7 04 24 14 00 00 00 	mov    DWORD PTR [esp],0x14
 8198e56:	31 c9                	xor    ecx,ecx
 8198e58:	89 0d 64 d2 42 08    	mov    DWORD PTR ds:0x842d264,ecx
 8198e5e:	e8 09 2c eb ff       	call   804ba6c <_ZNSt24__default_alloc_templateILb1ELi0EE8allocateEj@plt>
 8198e63:	a3 64 d2 42 08       	mov    ds:0x842d264,eax
 8198e68:	31 d2                	xor    edx,edx
 8198e6a:	83 fb 01             	cmp    ebx,0x1
 8198e6d:	c7 40 04 00 00 00 00 	mov    DWORD PTR [eax+0x4],0x0
 8198e74:	c7 00 00 00 00 00    	mov    DWORD PTR [eax],0x0
 8198e7a:	a1 64 d2 42 08       	mov    eax,ds:0x842d264
 8198e7f:	89 15 68 d2 42 08    	mov    DWORD PTR ds:0x842d268,edx
 8198e85:	89 40 08             	mov    DWORD PTR [eax+0x8],eax
 8198e88:	a1 64 d2 42 08       	mov    eax,ds:0x842d264
 8198e8d:	89 40 0c             	mov    DWORD PTR [eax+0xc],eax
 8198e90:	0f 85 73 fe ff ff    	jne    8198d09 <_Z41__static_initialization_and_destruction_0ii+0x39>
 8198e96:	b8 80 d2 42 08       	mov    eax,0x842d280
 8198e9b:	ba 05 00 00 00       	mov    edx,0x5
 8198ea0:	c7 00 00 00 00 00    	mov    DWORD PTR [eax],0x0
 8198ea6:	4a                   	dec    edx
 8198ea7:	c7 40 04 00 00 00 00 	mov    DWORD PTR [eax+0x4],0x0
 8198eae:	c7 40 08 00 00 00 00 	mov    DWORD PTR [eax+0x8],0x0
 8198eb5:	83 c0 0c             	add    eax,0xc
 8198eb8:	83 fa ff             	cmp    edx,0xffffffff
 8198ebb:	75 e3                	jne    8198ea0 <_Z41__static_initialization_and_destruction_0ii+0x1d0>
 8198ebd:	e9 47 fe ff ff       	jmp    8198d09 <_Z41__static_initialization_and_destruction_0ii+0x39>
 8198ec2:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
 8198ec9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

08198ed0 <_GLOBAL__I__ZN12ITEM_MANAGERC2Ev>:
 8198ed0:	55                   	push   ebp
 8198ed1:	b9 ff ff 00 00       	mov    ecx,0xffff
 8198ed6:	89 e5                	mov    ebp,esp
 8198ed8:	83 ec 08             	sub    esp,0x8
 8198edb:	89 4c 24 04          	mov    DWORD PTR [esp+0x4],ecx
 8198edf:	c7 04 24 01 00 00 00 	mov    DWORD PTR [esp],0x1
 8198ee6:	e8 e5 fd ff ff       	call   8198cd0 <_Z41__static_initialization_and_destruction_0ii>
 8198eeb:	89 ec                	mov    esp,ebp
 8198eed:	5d                   	pop    ebp
 8198eee:	c3                   	ret
 8198eef:	90                   	nop
