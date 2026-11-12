# Quarter-loop equivalence proof (including all negative32-bit step values)

Original GetRealPoint25 return is signedlong32. Originalcmp3/jg at0812e776..77b clamps ONLY values>3 to4 after diagnostic; negatives inclINT_MIN are untouched. Candidate has the same >3 predicate and local4. No normal-step-domain assumption is used.

Target4:0812e796 mov4,eax; e79b subebx,eax (low32 wrap); e79d test eax; e79f SIGNEDjle skips to e7cf. Positive count copied toEBX e7a1, actual recursivePointChange25 at e7c7, DECEBX e7cc/JNE e7cd.
Target3:0812e83c mov3,eax; e841 subebx,eax; e843 test; e845 SIGNEDjle skip; positivecount EBX e847, call e86d, DEC e872/JNE e873.
Target2:0812e8c3 mov2,eax; e8c8 subebx,eax; e8ca test; e8cc SIGNEDjle skip; positivecount EBX e8d2, call e8f8, DEC e8fd/JNE e8fe.

Candidate sub(target,static_cast<int>(step)) uses unsignedint32 subtraction then two's-complement signed conversion, reproducing EAX's exact bits. while(count>0) applies the original signed pretest. Once count>0 is admitted, original DEC/JNE executes exactly count calls: all intermediate counts remain0..INT_MAX, so replacing post-DEC/JNE with while(count>0) has no overflow/sign discrepancy. Candidate decrements the CACHED count local, not GetRealPoint reread after recursivecalls, exactly as cached callee-saved EBX. No source count is mathematical/unbounded target-step.

At stepINT_MIN, target2/3/4 yield wrapped signed INT_MIN+target (<0), so original AND candidate makeZERO recursivecalls for the selected branch. Near INT_MIN values can instead yield INT_MAX theoreticalcalls: target2,stepINT_MIN+3; target3,stepINT_MIN+4; target4,stepINT_MIN+5. Those enormous loops are retained, NOT clamped/optimized away. quarter-loop-boundaries.json records wrap/test/theoreticaliterations without executing any applicationloop.

Branch selection remains original ordered control flow: fullEXP>=next and step<=3 chooses target4; ELSE EXP>=3q and step<=2 chooses3; ELSE EXP>=2q and step<=1 chooses2; ELSE EXP>=q and step<1 makes ONE direct call. A selected target4/3/2 with wrapped nonpositive count doesNOT fall through to lower quarters. The candidate's else-if chain preserves that. The firstquarter path has no count=1-step loop in the original and MUST retain exactlyONE call even atINT_MIN; candidate does.

At actualnext100, stepINT_MIN: EXP25 chooses one directquarter1 call; EXP50/75/100 select target2/3/4 and each skips after signedcounttest. No billion-iteration or applicationcalls were executed by this proofscript. Full instruction excerpt is quarter-loop-assembly.txt; entire source remains assembly.txt.

Other requested callback audit: HP candidate stores/caches val BEFORE BroadcastTargetPacket then compares cachedval to previousHP, reloading party/descriptor afterward. No GetPoint/hp reread replaces cachedval. Gradebonus95..98 SetPoint bonus then recursegrade and THEN GetPoint(originalbonus), matching target0812f4ef/0812eff4. DEF16 andpercent119/120 intentionally retain PRE-recursion cachedvalue instead. These distinctions are explicit in candidate.
