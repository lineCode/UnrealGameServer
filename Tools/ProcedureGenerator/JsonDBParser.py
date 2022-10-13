import json

class JsonDBParser:
     def __init__(self):
        self.tables = {}
        self.procedures = []

     def jsonparser(self , path):
        with open(path) as data:
             json_object = json.load(data)
             for jsontable in json_object['Table']:
                 self.tables[jsontable['name']] = Table(jsontable)
             for jsonprocedure in json_object['Procedure']:
                 self.procedures.append(Procedure(jsonprocedure, self.tables))

class Table:
    def __init__(self, table):
        self.name = table['name']
        self.columns = {}
        for column in table['Column']:
           self.columns[column['name']] = ReplaceType(column['type'])

class Procedure:
    def __init__(self, procedure, tables):
        name = procedure['name']
        if name.startswith('Proc'):
            self.name = name[4:]
        else:
            self.name = name
        self.params = []
        for param in procedure['Param']:
                self.params.append(Param(param))
        body = procedure['Body']
        self.columns = ParseColumns(body, tables)
        self.questions = MakeQuestions(self.params)

class Param:
    def __init__(self, param):
        name = param['name'].replace('@', '')
        self.name = name[0].upper() + name[1:]
        self.type = ReplaceType(param['type'])

class Column:
    def __init__(self, name, type):
        self.name = name[0].upper() + name[1:]
        self.type = type

def ParseColumns(object, tables):
    columns = []
    query = object
    select_idx = max(query.rfind('SELECT'), query.rfind('select'))
    from_idx = max(query.rfind('FROM'), query.rfind('from'))
    if select_idx >= 0 and from_idx > 0 and from_idx > select_idx:
        table_name = query[from_idx+len('FROM') : -1].strip().split()[0]
        table_name = table_name.replace('[', '').replace(']', '').replace('dbo.', '')
        table = tables.get(table_name)
        words = query[select_idx+len('SELECT') : from_idx].strip().split(",")
        for word in words:
            column_name = word.strip().split()[0]
            columns.append(Column(column_name, table.columns[column_name]))
    elif select_idx >= 0:
        word = query[select_idx+len('SELECT') : -1].strip().split()[0]
        if word.startswith('@@ROWCOUNT') or word.startswith('@@rowcount'):
            columns.append(Column('RowCount', 'int64'))
        elif word.startswith('@@IDENTITY') or word.startswith('@@identity'):
            columns.append(Column('Identity', 'int64'))
    return columns

def MakeQuestions(params):
    questions = ''
    if len(params) != 0:
        questions = '('
        for idx, item in enumerate(params):
            questions += '?'
            if idx != (len(params)-1):
                questions += ','
        questions += ')'
    return questions

def ReplaceType(type):
    if type == 'bool':
        return 'bool'
    if type == 'int':
        return 'int32'
    if type == 'bigint':
        return 'int64'
    if type == 'datetime':
        return 'TIMESTAMP_STRUCT'
    if type.startswith('nvarchar'):
        return 'nvarchar'
    return type