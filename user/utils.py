# users/utils.py
from suitcases.models import Category, Suitcase

def calculate_jaccard_similarity(user_categories:list[Category], suitcase_categories:list[Category]) -> tuple[float,float]:
    """
    Calculate Jaccard similarity between user selected categories and suitcase categories. (0-1 float)
    It is slighty modified to fit the use case that is to say it is A ∩ B / B
    """
    if len(user_categories) == 0:
        return 0.0,0.0
    if len(suitcase_categories) == 0:
        return 0.0,0.0

    matches = 0
    unique_matches = 0
    seen = []
    for suit_cat in suitcase_categories:
        for user_cat in user_categories:
            if user_cat == suit_cat:
                matches+= 1
                if not (user_cat in seen):
                    unique_matches += 1
                    seen.append(user_cat)
                    
    category_count= len(suitcase_categories) 

    overall_score = matches / category_count
    unique_score = unique_matches/ category_count
    
    return unique_score, overall_score 

def get_recommended_suitcases(user_categories:list[Category], max_recommendations:int=3):
    """ Get the recommended suitcases based on jaccard similarity """
    available_suitcases = Suitcase.objects.filter(rented=False).prefetch_related('categories')
    scored_suitcases = []
    for suitcase in available_suitcases:
        suitcase_categories = suitcase.categories.all()
        unique_score, overall_score = calculate_jaccard_similarity(user_categories, suitcase_categories) 

        scored_suitcases.append({'suitcase': suitcase, 'score': unique_score,"overall_score": overall_score, 'categories': suitcase_categories})

    scored_suitcases.sort(key=lambda x: x['overall_score'], reverse=True)
    return scored_suitcases[:max_recommendations]
