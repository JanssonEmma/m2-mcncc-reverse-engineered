from pathlib import Path
import ast
p=Path('work/pc_quest_storage')
class Encoding(ast.NodeTransformer):
 def visit_Call(self,node):
  self.generic_visit(node)
  if isinstance(node.func,ast.Attribute) and node.func.attr in ('read_text','write_text') and not any(k.arg=='encoding' for k in node.keywords):node.keywords.append(ast.keyword(arg='encoding',value=ast.Constant('utf-8-sig' if node.func.attr=='read_text' else 'utf-8')))
  return node
for name in ['stage.py','extract_metadata.py','show_inline.py','arg_ctor_proof.py','prove_inline.py','show_graph.py']:
 f=p/name;tree=Encoding().visit(ast.parse(f.read_text(encoding='utf-8-sig')));ast.fix_missing_locations(tree);f.write_text(ast.unparse(tree)+'\n',encoding='utf-8')
print('All remaining generator/evidence script text IO explicitlyUTF8; payload unchanged')
